#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <ctime>
#include <unordered_set>
#include <unordered_map>
#include <random>
#include <functional>
#include <chrono>

//#include "machine.cpp"

#include "machineController.cpp"

using namespace std;

void localSearch(pair<vector<Schedule>, int>& finalSchedule, const vector<vector<Task>>& jobList);

void printJobs(const vector<vector<Task>> &jobList){
    int numJobs = jobList.size(); // Tamanho da linha (número de jobs)
    int numMachines = jobList.at(0).size(); // Tamanho da coluna (número de máquinas)

    for(int i=0; i<numJobs; i++){
        for(int j=0; j<numMachines; j++){
            cout << jobList[i][j].machine << " " << jobList[i][j].time << " ";
        }
        cout << endl;
    }
}

void printSchedule(const vector<Schedule>& agenda) {
    // for(int i=0; i<agenda.size(); i++){
    //     cout << "Agendado: Trabalho " << agenda[i].task.job << " Trabalhojob " << agenda[i].job<< ", Tarefa " << agenda[i].task.k << endl;
    //     cout << "Máquina: " << agenda[i].task.machine << endl;
    //     cout << "Tempo de execução: " << agenda[i].task.time << endl;
    //     cout << endl;
    // }


    for(int i=0; i<agenda.size(); i++){
        printf("{%d, %d, %d}\n", agenda[i].task.job, agenda[i].task.k, agenda[i].task.machine);
    }
}

// Gera um agendamento aleatorio valido
void randomSchedule(const vector<vector<Task>>& jobList, vector<Schedule>& agenda) {
    int numJobs = jobList.size();
    int numMachines = jobList[0].size();


    vector<queue<Task>> aux(numJobs);
    for(int i=0; i<numJobs; i++){
        for(int j=0; j<numMachines; j++){
            aux[i].push(jobList[i][j]);
        }
    }

    bool allQueuesEmpty = false;
    while (!allQueuesEmpty) {
        allQueuesEmpty = true;

        for (int i = 0; i < numJobs; i++) {
            if (!aux[i].empty()) {
                allQueuesEmpty = false;

                // Seleciona aleatoriamente uma fila não vazia
                int randomIndex = rand() % numJobs;
                while (aux[randomIndex].empty()) {
                    randomIndex = rand() % numJobs;
                }

                // Adiciona o elemento do topo da fila ao vetor agenda
                Task task = aux[randomIndex].front();
                aux[randomIndex].pop();
                agenda.push_back({task.job, task});

                // Verifica se todas as filas estão vazias após adicionar o elemento
                bool allQueuesEmptyNow = true;
                for (int j = 0; j < numJobs; j++) {
                    if (!aux[j].empty()) {
                        allQueuesEmptyNow = false;
                        break;
                    }
                }

                // Se todas as filas estiverem vazias, interrompe o loop
                if (allQueuesEmptyNow) {
                    allQueuesEmpty = true;
                    break;
                }
            }
        }
    }

}

vector<pair<vector<Schedule>, int>> getRandomPopulation(const vector<vector<Task>>& jobList, int populationSize) {
    vector<pair<vector<Schedule>, int>> population;
    int numMachines = jobList[0].size();

    for (int i = 0; i < populationSize; i++) {
        vector<Schedule> individual;
        randomSchedule(jobList, individual);  // Gere um indivíduo aleatório usando sua função randomSchedule()

        MachineController mc(numMachines, individual, jobList);
        int totalTime = mc.getResultSchedule();  // Obtenha o tempo total usando a função getResultSchedule()

        population.push_back(make_pair(individual, totalTime));
    }

    return population;
}

bool replaceWorstSchedule(vector<pair<vector<Schedule>, int>>& population, const pair<vector<Schedule>, int>& filho) {
    auto maxCostIt = max_element(population.begin(), population.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    // Verifica se já existe algum elemento com o mesmo custo
    bool sameCostExists = any_of(population.begin(), population.end(), [&](const auto& individual) {
        return individual.second == filho.second;
    });

    if (maxCostIt != population.end() && filho.second < maxCostIt->second && !sameCostExists) {
        population.erase(maxCostIt);
        population.push_back(filho);
        return true;
    }
    return false;
}


void printPopulationCost(const vector<pair<vector<Schedule>, int>>& population) {
    for (const auto& individual : population) {
        cout << "Custo do agendamento: " << individual.second << endl;
    }
}

vector<pair<vector<Schedule>, int>> getXRandomSchedules(int x, const vector<pair<vector<Schedule>, int>>& population) {
    vector<pair<vector<Schedule>, int>> selectedSchedules;
    unordered_set<int> selectedIndices;

    // Configuração do gerador de números aleatórios
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, population.size() - 1);

    // Realiza a seleção aleatória dos agendamentos
    while (selectedSchedules.size() < x) {
        int randomIndex = dist(gen);
        if (selectedIndices.find(randomIndex) == selectedIndices.end()) {
            selectedSchedules.push_back(population[randomIndex]);
            selectedIndices.insert(randomIndex);
        }
    }

    return selectedSchedules;
}

pair<vector<Schedule>, int> crossover(const vector<pair<vector<Schedule>, int>>& parents, int x, int y, const vector<vector<Task>>& jobList) {
    int numMachines = jobList[0].size();
   
    vector<Schedule> parent1 = parents[0].first;
    vector<Schedule> parent2 = parents[1].first;
    

    vector<Schedule> temp1(parent1.begin() + x, parent1.begin() + y + 1);
    vector<Schedule> temp2(parent2.begin() + x, parent2.begin() + y + 1);

    int insertIndex1 = x;

    for (const Schedule& schedule : parent2) {
        auto it1 = find_if(temp1.begin(), temp1.end(), [&](const Schedule& s) { return s.job == schedule.job && s.task.k == schedule.task.k; });
        auto it2 = find_if(temp2.begin(), temp2.end(), [&](const Schedule& s) { return s.job == schedule.job && s.task.k == schedule.task.k; });

        if (it1 != temp1.end()) {
            parent1[insertIndex1] = *it1;
            insertIndex1++;
            temp1.erase(it1);
        }

        if (it2 != temp2.end()) {
            // Não atualizamos parent2
            temp2.erase(it2);
        }

        if (insertIndex1 > y) {
            break;  // Cruzamento concluído
        }
    }

    pair<vector<Schedule>, int> offspring;
    MachineController mc1(numMachines, parent1, jobList);
    int result1 = mc1.getResultSchedule();

    offspring.first = parent1;
    offspring.second = result1;

    return offspring;
}

void mutation(pair<vector<Schedule>, int>& filho, float alpha) {
    int max = filho.first.size() - 2;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, max);
    std::uniform_real_distribution<> probDis(0.0, 1.0);

    int mutationIndex = dis(gen);

    // Verificar a probabilidade de mutação com base em alpha
    if (probDis(gen) <= alpha) {
        // Verificar se os schedules são de jobs diferentes
        if (filho.first[mutationIndex].job != filho.first[mutationIndex + 1].job) {
            // Realizar o swap entre os schedules
            swap(filho.first[mutationIndex], filho.first[mutationIndex + 1]);
        }
    }
}

void getXY(int& x, int& y, int tamanhoShedule) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distX(0, tamanhoShedule - 2); // Define o intervalo [0, tamanhoShedule - 2] para x
    std::uniform_int_distribution<> distY(1, tamanhoShedule - 1); // Define o intervalo [1, tamanhoShedule - 1] para y

    x = distX(gen);
    y = distY(gen);

    if (x >= y) {
        std::swap(x, y);
    }
}

pair<vector<Schedule>, int> getBestSchedule(const vector<pair<vector<Schedule>, int>>& population) {
    auto minCostIt = min_element(population.begin(), population.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    if (minCostIt != population.end()) {
        return *minCostIt;
    }

    // Se o vetor estiver vazio, retorna um par vazio
    return make_pair(vector<Schedule>(), 0);
}

//Algoritmo Genetico
pair<vector<Schedule>, int> geneticHeuristic(const vector<vector<Task>>& jobList, int iterations){
    int numMachines = jobList[0].size();
    int numJobs = jobList.size();
    vector<pair<vector<Schedule>, int>> population = getRandomPopulation(jobList, numJobs*numMachines);
    // cout << "Populacao inicial" << endl;
    // printPopulationCost(population);

    float alpha=1;
    for(int i=0; i<iterations; i++){
        alpha*=0.999;// Comecar com muitas mutacoes e ir diminuindo com o tempo

        if(alpha<0.1){
            alpha=0.4;//Reiniciara a mutaçao
        }
        vector<pair<vector<Schedule>, int>> pais = getXRandomSchedules(2, population);
        int x,y;
        getXY(x,y,pais[0].first.size());
        
        // Gera dois filhos
        pair<vector<Schedule>, int>filho0 = crossover(pais, x, y, jobList);
        swap(pais[0], pais[1]);
        pair<vector<Schedule>, int>filho1 = crossover(pais, x, y, jobList);
        mutation(filho0, alpha);
        mutation(filho1, alpha);

        replaceWorstSchedule(population, filho0);
        replaceWorstSchedule(population, filho1);
    }

    cout << endl;
    // cout << "Populacao Final" << endl;
    // printPopulationCost(population);

    pair<vector<Schedule>, int> bestSchedule = getBestSchedule(population);
    return bestSchedule;
    
}

void localSearch(pair<vector<Schedule>, int>& finalSchedule, const vector<vector<Task>>& jobList) {
    int numMachines = jobList[0].size();
    int numJobs = jobList.size();
    pair<vector<Schedule>, int> currentSchedule = finalSchedule;
    bool improve = true;
    int nTasks = finalSchedule.first.size();


    while(improve){
        improve = false;
        for(int i = 0; i<nTasks; i++){
            int jobTask = currentSchedule.first[i].task.job;
            int kTask = currentSchedule.first[i].task.k;

            // Encontra as posicoes x,y que aquela task pode movimentar sem tornar o agendamento incompativel
            int x=-1; int y = -1;
            if((kTask>0) && (kTask<numMachines)){
                for(int j=0; j<nTasks; j++){
                    if((x!=-1)&&(y!=-1)) break;

                    int c_jobTask = currentSchedule.first[j].task.job;
                    int c_kTask = currentSchedule.first[j].task.k;

                    if(c_jobTask==jobTask){
                        if(c_kTask==(kTask-1))x=j;
                        if(c_kTask==(kTask+1))y=j;
                    }
                }
            }

           
            for(int j = x + 1; (j < y) && (j < nTasks) && (y != -1); j++){

                pair<vector<Schedule>, int> aux = currentSchedule;
                swap(aux.first[i], aux.first[j]);

                //Verifica se é valido
                for(int k=x+1; k<y; k++){

                }



                MachineController mc(numMachines, aux.first, jobList);
                int newCost = mc.getResultSchedule();
                aux.second = newCost;

                if(newCost<currentSchedule.second){
                    currentSchedule = aux;
                    improve = true;

                }
            }
    
        }
    }

    finalSchedule = currentSchedule;
}

int main(){
    srand(time(0));
    //Ignoramos as 4 primeiras linhas que sao comentarios
    string line;
    for (int i = 0; i < 4; ++i)
        getline(cin, line);

    //Le os dados do arquivo
    int jobs, machines;
    cin >> jobs >> machines;
    cout << jobs << " " << machines << endl;

    vector<vector<Task>> jobList(jobs, vector<Task>(machines)); //Onde salvaremos as informacoes do jobs
    for(int i=0; i<jobs; i++){
        for(int j=0; j<machines; j++){
            int m, time;
            cin >> m >> time;
            jobList[i][j].k=j;
            jobList[i][j].machine=m;
            jobList[i][j].time=time;
            jobList[i][j].job=i;
        }
    }

    //printJobs(jobList);

    // vector<Schedule> agenda;
    // randomSchedule(jobList, agenda);
    //printSchedule(agenda);

    auto startTime = std::chrono::steady_clock::now();
    pair<vector<Schedule>, int> bestSchedule = geneticHeuristic(jobList, 1000000);
    cout << "Melhor Schedule tem custo: " << bestSchedule.second << endl;
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    std::cout << "Tempo de execução: " << duration.count() << " segundos" << std::endl;
    
    MachineController mc(machines, bestSchedule.first, jobList );
    mc.getResultSchedule();
    mc.save("schedule.txt");

   

    return 0;
}