from blood_utils import print_with_timestamp
from discord.ext import tasks, commands
import os

LOCKFILE_PATH = f"""{os.environ["TEMP"]}/BloodRenegade/DiscordBot"""
LOCKFILE_NAME = f"{LOCKFILE_PATH}/.lockfile"


class RebootBot(commands.Cog):

    def __init__(self, bot):
        self.bot = bot

        if not os.path.isdir(LOCKFILE_PATH):
            os.makedirs(LOCKFILE_PATH)

        with open(LOCKFILE_NAME, "w"):
            print_with_timestamp(f"Created lockfile at path {LOCKFILE_PATH}")

        self.check_lockfile.start()
        print_with_timestamp("RebootBot cog started")

    def cog_unload(self):
        self.check_lockfile.cancel()
        
    @tasks.loop(seconds=15.0)
    async def check_lockfile(self):
        if not os.path.isfile(LOCKFILE_NAME):
            print_with_timestamp("Lockfile deleted, rebooting...")
            await self.bot.close()


def setup(bot):
    bot.add_cog(RebootBot(bot))
