#!/usr/bin/env python3

import numpy as np
import os
import shutil
import pandas as pd
import matplotlib.pyplot as plt
from pylab import cm
import matplotlib as mpl


FIG_COUNT = 0
IMG_FILE_EXTENSION = ".png"
IMG_TRANSPARENT = False
OUTPUT_PATH = "figures"
DATA_FILE_NAME_1 = "2022-05-01 18:22:33.719_sync_data.csv"
DATA_FILE_NAME_2 = "2022-05-01 18:22:33.719_blink_data.csv"
IMG_X_SIZE_IN = 10
IMG_Y_SIZE_IN = 5


def create_folder(folder_name):
    if os.path.exists(folder_name):
        shutil.rmtree(folder_name)
    os.makedirs(folder_name)


def next_fig_num():
    global FIG_COUNT
    FIG_COUNT = FIG_COUNT + 1
    return FIG_COUNT


def update_ax_params(ax):
    name = "tab10"
    cmap = cm.get_cmap(name)
    colors = cmap.colors
    ax.set_prop_cycle(color=colors)

    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)

    ax.xaxis.set_tick_params(which='major', size=2, width=1, direction='out')
    ax.yaxis.set_tick_params(which='major', size=2, width=1, direction='out')

    ax.tick_params(axis='x', labelsize=15, pad=0.4)
    ax.tick_params(axis='y', labelsize=15, pad=0.4)

    ax.linesize = 1


def update_plt_params():
    mpl.rcParams['font.family'] = 'sans-serif'
    # plt.rc('text', usetex=True)
    plt.rcParams['font.size'] = 20
    # plt.rcParams["legend.handlelength"] = 0.8
    # plt.rc('legend', fontsize=20)
    # plt.rcParams["legend.labelspacing"] = 0.2
    # plt.rcParams["legend.columnspacing"] = 0.2
    # plt.rcParams["legend.handletextpad"] = 0.5
    # plt.rc('text.latex', preamble=r'\usepackage[cm]{sfmath}')


def clear_plt():
    plt.cla()
    plt.clf()
    plt.close('all')
    fig, ax2d = plt.subplots()
    fig.set_size_inches(IMG_X_SIZE_IN, IMG_Y_SIZE_IN)
    # plt.tight_layout()
    update_ax_params(ax2d)
    return fig, ax2d


def save_fig(fig_name):
    global IMG_FILE_EXTENSION
    global IMG_TRANSPARENT
    full_name = "Figure_" + str(next_fig_num()) + "_" + fig_name
    plt.savefig(OUTPUT_PATH + "/" + full_name +
                IMG_FILE_EXTENSION, transparent=IMG_TRANSPARENT)


def read_to_df(file_name):
    df = pd.DataFrame()
    try:
        df = pd.read_csv(file_name)
    except FileNotFoundError:
        print("No file: " + file_name)
    return df


def get_time_array(df):
    time_arr = pd.to_datetime(df["time"])
    return time_arr


def time_to_interval_arr(tm):
    init_time = tm[0]
    duration_arr = []
    for i in tm:
        duration = i - init_time
        duration_in_s = duration.total_seconds()*1000
        duration_arr.append(duration_in_s)
    duration_arr = np.array(duration_arr)
    return duration_arr


def plot_time_duration(df):
    print("\nPlotting time interval")

    tm_arr = get_time_array(df)
    dur_arr = time_to_interval_arr(tm_arr)

    fig, ax2d = clear_plt()
    ax2d.set_xlabel("Index")
    ax2d.set_ylabel("Time (ms)")
    dur_idx = np.arange(0, len(dur_arr), 1)
    ax2d.plot(dur_idx, dur_arr, linewidth=1, c='k')
    save_fig("time_interval")


def plot_time_series(df, ax2d, num):
    print("\nPlotting time series")

    tm_arr = get_time_array(df)
    dur_arr = time_to_interval_arr(tm_arr)

    if (ax2d.collections) == False:
        fig, ax2d = clear_plt()

    dur_idx = np.ones(len(dur_arr))

    c_arr = ['b', 'g']
    for value in dur_arr:
        x_val = [value, value]
        y_val = [num-1, num]
        ax2d.plot(x_val, y_val, c_arr[num-1])

    #ax2d.scatter(dur_arr, dur_idx, c='r')
    ax2d.set_xlabel("Time (ms)")
    ax2d.set_ylabel("Flash (I/O)")
    save_fig("time_series")
    return ax2d


def plot_period(df, ax2d):
    print("\nPlotting period")

    tm_arr = get_time_array(df)
    dur_arr = time_to_interval_arr(tm_arr)

    if ('period' in df.columns) == False:
        print('period not found')
        return
    period_arr = (df["period"]).to_numpy()

    if (ax2d.lines) == False:
        fig, ax2d = clear_plt()

    ax2d.scatter(dur_arr, period_arr, linewidth=4)
    ax2d.plot(dur_arr, period_arr, linewidth=2)

    ax2d.set_xlabel("Time (ms)")
    ax2d.set_ylabel("Period (ms)")
    save_fig("period_per_flash")
    return ax2d


def plot_period_shift(df, ax2d):
    print("\nPlotting phase shift")

    tm_arr = get_time_array(df)
    dur_arr = time_to_interval_arr(tm_arr)

    if ('period_adjust' in df.columns) == False:
        print('period_adjust not found')
        return
    period_arr = (df["period_adjust"]).to_numpy()

    if (ax2d.lines) == False:
        fig, ax2d = clear_plt()

    ax2d.scatter(dur_arr, period_arr, linewidth=4)
    ax2d.plot(dur_arr, period_arr, linewidth=2)

    ax2d.set_xlabel("Time (ms)")
    ax2d.set_ylabel("Period shift (ms)")
    save_fig("period_shift")
    return ax2d


def plot_voltage(df):
    print("\nPlotting voltage")

    tm_arr = get_time_array(df)
    dur_arr = time_to_interval_arr(tm_arr)
    if ('V' in df.columns) == False:
        print('V not found')
        return
    v_arr = (df["V"]).to_numpy()
    fig, ax2d = clear_plt()

    ax2d.scatter(dur_arr, v_arr, linewidth=4, c='r')
    ax2d.plot(dur_arr, v_arr, linewidth=2, c='k')

    ax2d.set_xlabel("Time (ms)")
    ax2d.set_ylabel("Voltage")
    save_fig("voltage")


create_folder(OUTPUT_PATH)
df_1 = read_to_df(DATA_FILE_NAME_1)
df_2 = read_to_df(DATA_FILE_NAME_2)

update_plt_params()

if df_1.empty == False:
    plot_time_duration(df_1)

    fig, ax = clear_plt()
    ax = plot_period(df_1, ax)
    plot_period(df_2, ax)

    fig, ax = clear_plt()
    ax = plot_period_shift(df_1, ax)
    plot_period_shift(df_2, ax)

    fig, ax = clear_plt()
    ax = plot_time_series(df_1, ax, 1)
    plot_time_series(df_2, ax, 2)

    plot_voltage(df_1)
