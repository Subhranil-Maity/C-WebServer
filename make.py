#!/usr/bin/env python3
import os
import hashlib
import sys

src_dir = "src"
build_dir = "build"
compiler = "gcc"
c_args = "-Wall -Wextra -Werror -Wpedantic -g"
output = "output"

h = hashlib.sha1()
comp_data = []
comp_data_path = f"{build_dir}/comp_data.txt"


def load_data(path: str) -> list[tuple[str, str]]:
    if not os.path.exists(path):
        return []
    with open(path, 'r') as file:
        return [tuple(line.strip().split()) for line in file.readlines()]


def save_data(path: str, datas: list[tuple[str, str]]) -> bool:
    with open(path, 'w') as file:
        for data in datas:
            file.write(f"{data[0]} {data[1]}\n")


def hash_file(path: str) -> str:
    with open(path, 'rb') as file:
        h.update(file.read())
    return h.hexdigest()


def log(msg: str) -> None:
    print(f"[LOG] {msg}")


def run(cmd: str) -> bool:
    result: int = os.system(cmd)
    status: int = result >> 8
    return status == 0


def compile_obj(name: str) -> bool:
    hash = hash_file(f"{src_dir}/{name}")

    if (name, hash) in comp_data and os.path.exists(f"{build_dir}/{name}.o"):
        log(f"{name} is up to date")
        return True

    log(f"Compiling {name} -> {name}.o [cached]")
    comp_data.append((name, hash))
    return run(f"{compiler} {c_args} -c {src_dir}/{name} -o {build_dir}/{name}.o")


def build_project(files: list[str]) -> bool:
    # for file in files:
    #     m = hash_file(f"{src_dir}/{file}.c")
    #     log(f"{file}: {m}")
    # return
    for file in files:
        if not compile_obj(file):
            return False
    log(f"Linking {files} -> {output}.out")
    run(f"{compiler} {build_dir}/*.o -o {build_dir}/{output}.out")
    return True


def get_all_source(path: str, exclude: list[str] = None) -> list[str]:
    if exclude is None:
        exclude = []
    files = []
    for file in os.listdir(path):
        if file not in exclude and file.endswith(".c"):
            files.append(file)
    return files


def build() -> None:

    global comp_data
    if not os.path.exists(build_dir):
        os.mkdir(build_dir)
    comp_data = load_data(comp_data_path)
    # build_project(["main", "web_log", "web_server"])
    build_project(get_all_source(src_dir))
    save_data(comp_data_path, comp_data)

def clean() -> None:
    if build_dir == "/":
        log("Cannot delete root directory")
        exit(1)
    os.system(f"rm -rf {build_dir}")
    log(f"Deleted {build_dir}")
    exit(0)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        build()
    elif sys.argv[1] == "clean":
        clean()
    elif sys.argv[1] == "build":
        build()
    elif sys.argv[1] == "run":
        build()
        log(f"Running {output}.out\n=================================")
        os.system(f"./{build_dir}/{output}.out")
        log("=================================")


