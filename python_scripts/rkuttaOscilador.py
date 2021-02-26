# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import matplotlib.pyplot as pyplot
import numpy as np
import os
import pandas as pandas
import sys


# %%
# import data
def ler_dados():
    data = []
    data.append(pandas.read_csv(".\\kutta.txt", header=None).to_numpy())
    return data


def main_func():
    data = ler_dados()
    shape = data[0].shape
    user_input = float(input("\n Frequencia do oscilador:  "))
    total_linhas = shape[0]
    total_colunas = shape[1]


# %%
if __name__ == "__main__":
    main_func()
