import subprocess
import time
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm
import pickle as pk

translate_key = {
    "exe1": "heuristic solution",
    "exe2": "local search",
    "exe3": "exaustive search [01]",
    "exe4": "exaustive search [02]",
}

def get_n_sizes(file):
    with open(f"test/{file}") as f:
        content = f.read()
    return content

def run_with_input(exe, file):
    with open(f"test/{file}") as f:
        start = time.perf_counter()
        proc = subprocess.run([f"exe/{exe}"], input=f.read(), text=True, capture_output=True)
        end = time.perf_counter()
        return (int(proc.stdout), (end - start))

def save_results(results, path):
    with open(path, 'wb') as f:
        pk.dump(results, f)

def load_results(path):
    with open(path, 'rb') as f:
        loaded_dict = pk.load(f)
        return loaded_dict

def run_multiple_times(exe_list, file_list, save=False, path="./results/hle.pkl"):
    res = dict()
    for exe in exe_list:
        res[exe] = {"result": [], "clock": []}
        #print("running {0}".format(translate_key[exe]))
        for i in range(len(file_list)):
            file = file_list[i]
            result, clock = run_with_input(exe, file)
            res[exe]["result"].append(result)
            res[exe]["clock"].append(clock)
    
    if save:
        save_results(res, path)

    return res

def generate_regression(x, y, degree, start, end):
    mymodel = np.poly1d(np.polyfit(x, y, degree))
    new_x = np.linspace(start, end, end-start)
    return [new_x, mymodel(new_x)]

def project_results(results, start, end):
    res = dict()
    for algorithm in results:
        x = results[algorithm]['result']
        y = results[algorithm]['clock']
        x_projected, y_projected = generate_regression(x, y, 2, start, end)
        res[algorithm] = {'result': x_projected, 'clock': y_projected}
    return res


def print_lines(results, n_size, title, x_label, y_label, value):
    for key in results:
        x = n_size
        y = results[key][value]
        plt.plot(x, y, label=translate_key[key])
    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.legend()
    plt.show()