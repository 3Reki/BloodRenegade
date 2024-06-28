from blood_utils import print_with_timestamp
import discord
import logging
import dotenv
import os
import json

dotenv.load_dotenv()
token = str(os.getenv("TOKEN"))

logging.basicConfig()
intents = discord.Intents.default()
intents.message_content = True

bot = discord.Bot(intents=intents)

bot.subscriber_lst = []
bot.dev_mode = False


def load_info():
    if os.path.isfile("save.json"):
        with open("save.json", "r") as save_file:
            jenkins_infos = json.loads(save_file.readline())

            bot.subscriber_lst = jenkins_infos["Subscribers"]


def save_info():
    with open("save.json", "w") as save_file:
        json_info = {
            "Subscribers": bot.subscriber_lst,
        }
        save_file.write(json.dumps(json_info))
        print_with_timestamp("Saved jenkins_info.json")


@bot.event
async def on_ready():
    load_info()
    print_with_timestamp(f'Logged in as {bot.user}')


@bot.event
async def on_message(message):
    if message.author == bot.user:
        return

    if message.content.startswith('fuck'):
        await message.channel.send('Nathan')


@bot.slash_command(guild_ids=[1087780275989270538], name="blood", description="Responds Renegade")
async def blood(ctx):
    await ctx.respond("Renegade")


@bot.slash_command(guild_ids=[1087780275989270538])
async def aled(ctx):
    await ctx.respond("Bonne chance frérot :thumbsup:")


@bot.slash_command(guild_ids=[1087780275989270538], name="subscribe", description="Subscribe to binaries status")
async def subscribe(ctx):
    if ctx.author.id in bot.subscriber_lst:
        bot.subscriber_lst.remove(ctx.author.id)
        await ctx.respond("Removed from subscribers. You will no longer receive a ping from binaries status update.")
    else:
        bot.subscriber_lst.append(ctx.author.id)
        await ctx.respond("Added to subscribers. You will now be pinged whenever binaries status change.")

    save_info()


@bot.slash_command(guild_ids=[1087780275989270538])
async def devmode(ctx):
    bot.dev_mode = not bot.dev_mode
    if bot.dev_mode:
        await ctx.respond("Dev mode enabled")
    else:
        await ctx.respond("Dev mode disabled")


# @bot.command(guild_ids=[1087780275989270538])
# async def gtn(ctx):
#     """A Slash Command to play a Guess-the-Number game."""
#
#     await ctx.respond('Guess a number between 1 and 10.')
#     guess = await bot.wait_for('message', check=lambda message: message.author == ctx.author)
#
#     if int(guess.content) == 5:
#         await ctx.send('You guessed it!')
#     else:
#         await ctx.send('Nope, try again.')
#
#
# @bot.command(guild_ids=[1087780275989270538])
# async def hello(ctx):
#     embed = discord.Embed(
#         title="My Amazing Embed",
#         description="Embeds are super easy, barely an inconvenience.",
#         color=discord.Colour.blurple(),  # Pycord provides a class with default colors you can choose from
#     )
#     embed.add_field(name="A Normal Field",
#                     value="A really nice field with some information. **The description as well as the fields support markdown!**")
#
#     embed.add_field(name="Inline Field 1", value="Inline Field 1", inline=True)
#     embed.add_field(name="Inline Field 2", value="Inline Field 2", inline=True)
#     embed.add_field(name="Inline Field 3", value="Inline Field 3", inline=True)
#
#     embed.set_footer(text="Footer! No markdown here.")  # footers can have icons too
#     embed.set_author(name="Pycord Team", icon_url="https://example.com/link-to-my-image.png")
#     # embed.set_thumbnail(url="file:///D:/BloodRenegade/GameIcon.png")
#     # embed.set_image(url="file:///D:/BloodRenegade/keyart.png")
#
#     await ctx.respond("Hello! Here's a cool embed.", embed=embed)  # Send the embed with some text


bot.load_extension('jenkins_handler')
bot.load_extension('reboot_bot')
bot.run(token)
