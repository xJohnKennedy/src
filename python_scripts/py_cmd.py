import secao_de_poicare_plots as poincare
import plano_fase_plots as planoFase
import os

if __name__ == "__main__":
    msg: str = "\n" + "[1]: plotar secao de Poincare\n" + \
        "[2]: plotar plano fase\n" + "[3]: plotar resposta no tempo\n" + \
            "[4]: plotar [1],[2]\n"+"[5]: plotar [1],[2],[3]\n"

    print(msg)
    user_input = int(input("\nExecutar:  "))

    if user_input == 1:
        poincare.main_func()
        os.system("pause")
    elif user_input == 2:
        planoFase.main_func()
        os.system("pause")
    elif user_input == 4:
        poincare.main_func()
        planoFase.main_func()
        os.system("pause")