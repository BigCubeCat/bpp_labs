import json


def change_param(txt, param, new_value):
    return txt.replace("@" + param, new_value)


def change_name(txt, name):
    return change_param(txt, "name", name)


def change_path(txt, path):
    return change_param(txt, "path", path)


def read_config(filename, name, path) -> list:
    with open(filename, "r", encoding="utf-8") as file:
        txt = file.read()
        txt = change_name(txt, name)
        txt = change_path(txt, path)
        return list(json.loads(txt))
