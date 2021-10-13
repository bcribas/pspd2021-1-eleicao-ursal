### SHOULD BE RUN FROM REPOSITORY ROOT FOLDER, NOT FROM 'reltorios' FOLDER
import subprocess
import queue


def run_benchmark(program):
    command = f"BENCHMARKBINARY={program}"

    _ = subprocess.run(["make", "clean"], stdout=subprocess.PIPE)
    result = subprocess.run(
        ["make", "-s", "benchmark", command],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    progman_order = result.stdout.decode("utf-8").split("\n")

    run_times = result.stderr.decode("utf-8").split("\n")
    run_times_queue = queue.Queue()
    for o in run_times:
        run_times_queue.put(o)

    ret = {}
    last_key = ""
    progman_order.pop()
    for prog in progman_order:

        if prog == "":

            data = {
                "real": float(run_times_queue.get().split(" ")[1]),
                "user": float(run_times_queue.get().split(" ")[1]),
                "sys": float(run_times_queue.get().split(" ")[1]),
            }

            ret[last_key] = data
        else:
            last_key = prog.split("/")[-1]

    return ret


def print_result(prog, data):

    print(f"\n{prog}")

    files = {}
    shows = {}
    for run in data:
        for f, val in run.items():
            if f not in files:
                files[f] = []

            files[f].append(val)

    for f, runs in files.items():
        show_str = [f'{d["real"]}, {d["user"]}, {d["sys"]}' for d in runs]
        show_str = ", ".join(show_str)
        shows[f] = f"\t{' ' * (16 - len(f))}{f}: {show_str}"

    order = [
        "file001-sample",
        "file009",
        "file002-sample",
        "file003",
        "file004",
        "file005-sample",
        "file006",
        "file007-sample",
        "file008",
        "file010-big",
        "file-011-big",
    ]

    for f in order:
        print(shows[f])


def main():

    BASE_PATH = "relatorios/Gabriela-Geovana-Guilherme/build/"
    programs = [
        "paralelo_1",
        "paralelo_2",
        "paralelo_4",
        "paralelo_8",
        "paralelo_12",
        "paralelo_16",
        # "sequencial",
    ]

    benchmark_results = {}

    for p in programs:

        benchmark_results[p] = []

        for i in range(0, 4):
            print(f"Running {p} #{i + 1}...")
            result = run_benchmark(f"{BASE_PATH}{p}")

            if i == 0:
                continue  # discard first run

            benchmark_results[p].append(result)

    for p in programs:
        print_result(p, benchmark_results[p])


if __name__ == "__main__":
    main()
