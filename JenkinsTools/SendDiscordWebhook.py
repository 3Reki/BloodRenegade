from discord_webhook import DiscordWebhook
import sys


channel_url = "https://discord.com/api/webhooks/1210240678634987571/-WmkbJyL7J1Z-FPn_EhV_GdWeLqBpaU8rHNWFUBONF9jAjRC5p3EotDZY9JeFVOIIkCl"
content = sys.argv[1]
webhook = DiscordWebhook(url=channel_url, content=content)
response = webhook.execute()