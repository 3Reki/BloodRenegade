from jenkinsapi import jenkins
import os
import json

# nosubmit

# jenkins_url = "http://192.168.140.201:8080"
# username = "builder"
# password = "builder"
#
# server = jenkins.Jenkins(jenkins_url, username=username, password=password)
# job = server.get_job("BUILD_BINARIES")
# last_build = job.get_last_build()
# print(last_build.get_status())

# if __name__ == '__main__':
#     if not os.path.isfile("jenkins_info.json"):
#         with open("jenkins_info.json", "w") as jenkins_conf_file:
#             x = {
#                 "JENKINS_VERBOSE_CHANNEL": 1168587005349863526,
#                 "message": 305445,
#             }
#             jenkins_conf_file.write(json.dumps(x))
#         exit(0)
#
#     with open("jenkins_info.json", "r") as jenkins_conf_file:
#         print(json.loads(jenkins_conf_file.readline()))

if __name__ == '__main__':
    lockfile_path = f"""{os.environ["TEMP"]}/BloodRenegade/DiscordBot"""
    lockfile_name = f"{lockfile_path}/.lockfile"

    if not os.path.isdir(lockfile_path):
        os.makedirs(lockfile_path)

    with open(lockfile_name, "w"):
        pass

    print("program start")
    while os.path.isfile(lockfile_name):
        continue

    print("program end")
