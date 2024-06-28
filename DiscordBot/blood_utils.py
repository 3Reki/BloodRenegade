from datetime import datetime


def get_datetime_prefix():
    return f"[{datetime.now()}]"


def print_with_timestamp(text):
    print(f"{get_datetime_prefix()} {text}")
