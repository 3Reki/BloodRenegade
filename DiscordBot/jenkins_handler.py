from blood_utils import print_with_timestamp
from enum import Enum
from discord.ext import tasks, commands
from jenkinsapi import jenkins
import jenkinsapi.constants
import discord
import asyncio
import json
import os


class Status(Enum):
    RUNNING = 1
    STOPPED = 2
    FAILED = 3


class JenkinsHandler(commands.Cog):

    def __init__(self, bot):
        jenkins_url = "http://192.168.140.201:8080"
        username = "builder"
        password = "builder"

        self.bot = bot
        self.verbose_channel = None
        self.status = Status.STOPPED

        with open("jenkins_info.json", "r") as jenkins_conf_file:
            jenkins_infos = json.loads(jenkins_conf_file.readline())

            self.jenkins_verbose_channel = jenkins_infos["JenkinsVerboseChannel"]
            self.status_message_id = jenkins_infos["StatusMessageId"]

        self.server = jenkins.Jenkins(jenkins_url, username=username, password=password)
        self.job_names = ["PERFORCE_GET", "BUILD_BINARIES", "PERFORCE_SUBMIT"]
        self.jobs = [self.server.get_job(name) for name in self.job_names]
        self.prev_ids = [job.get_last_buildnumber() for job in self.jobs]
        self.current_index = 0

        self.last_dev_mode = False
        self.check_for_devmode.start()

    def pause(self):
        self.wait_for_job_start.cancel()
        self.wait_for_job_end.cancel()
        self.refresh_status_message.cancel()
        self.save_jenkins_info()
        print_with_timestamp("Paused JenkinsHandler")

    def unpause(self):
        if not self.wait_for_job_start.is_running() and not self.wait_for_job_end.is_running():
            self.wait_for_job_start.start()
        if not self.refresh_status_message.is_running():
            self.refresh_status_message.start()
        print_with_timestamp("Unpaused JenkinsHandler")

    def cog_unload(self):
        self.wait_for_job_start.cancel()
        self.wait_for_job_end.cancel()
        self.refresh_status_message.cancel()
        self.check_for_devmode.cancel()
        self.save_jenkins_info()

    @tasks.loop(seconds=2)
    async def check_for_devmode(self):
        if self.last_dev_mode == self.bot.dev_mode:
            return

        self.last_dev_mode = self.bot.dev_mode
        if self.bot.dev_mode:
            self.pause()
        else:
            self.unpause()

    def save_jenkins_info(self):
        with open("jenkins_info.json", "w") as jenkins_conf_file:
            json_info = {
                "JenkinsVerboseChannel": self.jenkins_verbose_channel,
                "StatusMessageId": self.status_message_id,
            }
            jenkins_conf_file.write(json.dumps(json_info))
            print_with_timestamp("Saved jenkins_info.json")

    @commands.Cog.listener()
    async def on_ready(self):
        print_with_timestamp("Starting JenkinsHandler cog...")
        self.verbose_channel = self.bot.get_channel(self.jenkins_verbose_channel)

        if not self.wait_for_job_start.is_running() and not self.wait_for_job_end.is_running():
            self.wait_for_job_start.start()
        if not self.refresh_status_message.is_running():
            self.refresh_status_message.start()

        print_with_timestamp("JenkinsHandler cog started")

    def check_jenkins_pipeline_status(self):
        for job in self.jobs:
            last_build = job.get_last_build()

            if last_build.is_running():
                self.status = Status.RUNNING
                break

            build_status = last_build.get_status()
            if build_status != jenkinsapi.constants.STATUS_SUCCESS:
                self.status = Status.FAILED
                break

    async def rewrite_status_message(self):
        if self.status_message_id is not None:
            try:
                prev_message = await self.verbose_channel.fetch_message(self.status_message_id)
                if prev_message is not None:
                    await prev_message.delete()
            except discord.NotFound:
                print_with_timestamp(f"WARNING:Message {self.status_message_id} not found")

        if self.status is Status.STOPPED:
            status_desc = ":white_check_mark: You can pull safely !"
        elif self.status is Status.RUNNING:
            status_desc = ":warning: Binaries are being compiled, wait a moment before pulling..."
        else:
            status_desc = ":x: Last compile failed, Corentin will now proceed to cry :("

        embed = discord.Embed(
            title="Current binaries state",
            description=status_desc,
            color=discord.Colour.dark_magenta(),
        )
        new_message = await self.verbose_channel.send(embed=embed)

        self.status_message_id = new_message.id

        self.save_jenkins_info()

    @tasks.loop(seconds=0.5)
    async def refresh_status_message(self):
        if self.verbose_channel.last_message_id == self.status_message_id:
            return

        if self.verbose_channel.last_message.content == "reset status":
            self.status = Status.STOPPED

        await self.rewrite_status_message()

    @refresh_status_message.before_loop
    async def before_refresh_status_message(self):
        await self.bot.wait_until_ready()

        self.check_jenkins_pipeline_status()
        await self.rewrite_status_message()

    @tasks.loop(seconds=3)
    async def wait_for_job_start(self):
        if self.status == Status.RUNNING:
            current_id = self.jobs[self.current_index].get_last_buildnumber()
            if current_id != self.prev_ids[self.current_index]:
                self.prev_ids[self.current_index] = current_id
                self.wait_for_job_start.stop()

            return

        for i in range(0, len(self.jobs)):
            current_id = self.jobs[i].get_last_buildnumber()
            if current_id == self.prev_ids[i]:
                continue

            await self.verbose_channel.send("New binaries build started")

            if len(self.bot.subscriber_lst) > 0:
                subscriber_txt = "Subscribers :"
                for sub in self.bot.subscriber_lst:
                    subscriber_txt += f" <@{sub}>"

                await self.verbose_channel.send(subscriber_txt)
            self.current_index = i
            self.status = Status.RUNNING
            self.prev_ids[self.current_index] = current_id
            self.wait_for_job_start.stop()
            return

    @wait_for_job_start.after_loop
    async def on_wait_for_job_start_cancel(self):
        if not self.wait_for_job_start.is_being_cancelled():
            await self.verbose_channel.send("Starting job " + self.job_names[self.current_index])
            self.wait_for_job_end.start()

    @tasks.loop(seconds=1)
    async def wait_for_job_end(self):
        last_build = self.jobs[self.current_index].get_last_build()

        if last_build.is_running():
            return

        self.wait_for_job_end.stop()

        build_status = last_build.get_status()
        max_status_check_attempts = 25
        status_attempt = 0

        waiting_message = None
        if build_status is None:
            waiting_message = await self.verbose_channel.send("No build status found on Jenkins. Waiting...")
            last_build.poll()

        while build_status is None and status_attempt < max_status_check_attempts:
            print_with_timestamp("No build status found on Jenkins. Waiting...")
            await asyncio.sleep(0.5)
            build_status = last_build.get_status()
            status_attempt += 1

        # if waiting_message is not None: # comment for testing
        #     await waiting_message.delete()

        if status_attempt >= max_status_check_attempts:
            await self.verbose_channel.send("No build status found on Jenkins. Build result on discord may not be "
                                            "representative :warning:")
            print_with_timestamp("WARNING:Jenkins build status not found")
            self.current_index = 0
            build_status = jenkinsapi.constants.STATUS_SUCCESS

        if build_status == jenkinsapi.constants.STATUS_SUCCESS:
            if self.current_index + 1 < len(self.jobs):
                self.current_index += 1
                self.wait_for_job_start.start()
                return

            self.status = Status.STOPPED
            await self.verbose_channel.send("Build successful :white_check_mark:")

        else:
            self.status = Status.FAILED

            await self.verbose_channel.send("Build failed :x:")
            await self.verbose_channel.send("Status : " + str(build_status))

            if "BUILD" in self.job_names[self.current_index]:
                with open("../JenkinsTools/error.log", "r") as error_file:
                    error_log = error_file.read()
                    character_count = len(error_log)
                    if character_count < 4000:  # discord embed message max character limit
                        embed = discord.Embed(
                            title="Error Log",
                            description=error_log,
                            color=discord.Colour.dark_magenta(),
                        )
                        await self.verbose_channel.send(embed=embed)
                    else:
                        await self.verbose_channel.send("Error log exceeds max character limit. Sending as file : ",
                                                        file=discord.File("../JenkinsTools/error.log"))

                if os.path.isfile("../JenkinsTools/output.log"):
                    await self.verbose_channel.send("Full Output Log : ", file=discord.File("../JenkinsTools/output.log"))

        self.current_index = 0
        self.wait_for_job_start.start()


def setup(bot):
    bot.add_cog(JenkinsHandler(bot))

# if __name__ == '__main__':
#     server.build_job(j_name)
#     while True:
#         print('Waiting for build to start...')
#         if prev_id != job.get_last_buildnumber():
#             break
#         time.sleep(3)
#     print('Running...')
#     last_build = job.get_last_build()
#     while last_build.is_running():
#         time.sleep(1)
#     print(str(last_build.get_status()))
