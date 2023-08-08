import matplotlib.pyplot as plt
import numpy as np

def plot_schedule(file_path):
    with open(file_path, 'r') as file:
        # Ler o número de jobs e máquinas na primeira linha
        num_jobs, num_machines = map(int, file.readline().split())

        # Criar um dicionário para armazenar os tempos de início e término dos trabalhos por máquina
        machine_schedules = {machine: [] for machine in range(num_machines)}

        # Ler as linhas restantes e adicionar os tempos de início e término dos trabalhos no dicionário
        for line in file:
            job, task, machine, start_time, end_time = map(int, line.split())
            machine_schedules[machine].append((job, start_time, end_time))

    
    cmap = plt.get_cmap('tab10')
    job_colors = cmap(np.linspace(0, 1, num_jobs))

    # Plotar o gráfico
    for machine, schedule in machine_schedules.items():
        for job, start_time, end_time in schedule:
            plt.barh(machine, end_time - start_time, left=start_time, height=0.6, color=job_colors[job])

    # Configurar os eixos do gráfico
    plt.xlabel('Tempo')
    plt.ylabel('Máquina')
    plt.yticks(range(num_machines), range(num_machines))

    # Criar a legenda
    legend_elements = [plt.Rectangle((0, 0), 1, 1, color=job_colors[i], label=f'Job {i}') for i in range(num_jobs)]
    plt.legend(handles=legend_elements, bbox_to_anchor=(1.05, 1), loc='upper left')

    plt.tight_layout()

   
    plt.show()

# Exemplo de uso
file_path = 'schedule.txt'
plot_schedule(file_path)

