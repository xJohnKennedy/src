# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import numpy as np
import pandas as pandas
import math


# %%
# import data
def ler_dados():
    data = []
    data.append(pandas.read_csv(".\\kutta.txt", header=None).to_numpy())
    return data


# %%
def main_func():
    data = ler_dados()
    shape = data[0].shape
    freq = float(input("\n Frequencia do oscilador:  "))
    total_linhas = shape[0]
    osciladorXX = np.zeros((total_linhas, 1))
    osciladorYY = np.zeros((total_linhas, 1))
    for i in range(total_linhas):
        t = data[0][i]
        tt = t[0]
        osciladorXX[i] = math.sin(freq * tt)
        osciladorYY[i] = math.cos(freq * tt)
    data_temp = np.column_stack((data[0], osciladorXX))
    data = np.column_stack((data_temp, osciladorYY))

    data_pandas = pandas.DataFrame(data=data)

    data_pandas.to_csv('kutta_auto.txt',
                       index=False,
                       header=False,
                       sep=" ",
                       float_format="%+.15f")


# %%
if __name__ == "__main__":
    main_func()
