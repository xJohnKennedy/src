import secao_de_poicare_plots as poincare

if __name__ == "__main__":
    msg: str = "\n" + "[1]: plotar secao de Poincare\n" + \
        "[2]: plotar plano fase\n" + "[3]: plotar resposta no tempo\n" + \
            "[4]: plotar [1],[2]\n"+"[5]: plotar [1],[2],[3]\n"

    print(msg)
    user_input = int(input("\nExecutar:  "))

    if user_input == 1:
        poincare.main_func()