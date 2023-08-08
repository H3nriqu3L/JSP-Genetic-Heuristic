#include "machine.cpp"
#include <iostream>

using namespace std;

class MachineController {
public:
    MachineController(int numMachines, const vector<Schedule> &agenda, const vector<vector<Task>> &jobList): jobList(jobList){
        machines.resize(numMachines);
        machineJobList.resize(numMachines); //Fara o Shedule de cada Machine
        numberTasks = agenda.size();
        numberMac = numMachines;
        for(int i=0; i<numberTasks; i++){
            int machineTask = agenda[i].task.machine; //Qual a maquina destinada aquela Task
            Task task = agenda[i].task;
            machineJobList[machineTask].push(task);
        }
        
        for (int i = 0; i < numMachines; i++) {
            Machine machine(machineJobList[i], i);
            //cout << "Em machines[" <<i<<"] adicionei vector de task com k" << machineJobList[i].front().k << endl;
            machines[i] = machine;

        }
        //cout << "Contrutor Passou" << endl;
    }

    
   

    int getResultSchedule(){ 
        //cout << "Entrei getResultSchedule" << endl;
        scheduleTasks();


        int time=machines[0].getCurrentTime();

        for(int i=1; i<numberMac; i++){
            int aux = machines[i].getCurrentTime();
            if(aux>time){
                time = aux;
            }
        }

        // for(int i=0; i<numberMac; i++){
        //     machines[i].printProcessedTasks();
        // }
        custo = time;
        return time;
    }

    void save(const std::string& nome_arquivo) {
        std::ofstream arquivo(nome_arquivo);

        if (!arquivo.is_open()) {
            std::cout << "Erro ao abrir o arquivo." << std::endl;
            return;
        }

        arquivo << jobList.size() << " " << numberMac << std::endl;

        for (int i = 0; i < numberMac; i++) {
            machines[i].saveProcessedTasks(arquivo);
        }

        arquivo.close();
    }


private:
    const vector<vector<Task>> jobList;
    vector<Machine> machines;  
    vector<queue<Task>> machineJobList; //Cria a agenda separado por maquinas
    int numberTasks;
    int numberMac;
    int custo;

    Task getPreviousTask(Task task){
        int kTask = task.k; 
        int jobTask = task.job;

        // Percorre o jobList para encontrar a tarefa anterior
        
        if((kTask-1)>=0){
            Task previousTask = jobList[jobTask][kTask - 1];

            //printf("A previous Task de {%d, %d, %d} é {%d, %d, %d}\n", task.job, task.k, task.machine, previousTask.job, previousTask.k, previousTask.machine);
            return previousTask;
        }

        cout << "Nao achei a previous task" << endl;    //Se estiver funcionando corretamente esse cout nunca sera executado
        return { -1, -1, -1, -1 };
    }

    void scheduleTasks(){
        while(numberTasks>0){
            for(int i=0; i<numberMac; i++){
                if(numberTasks==0) break;
                if(machines[i].isProcessDone()) continue;

                Task nextTask = machines[i].getNextTask();

                //Se a k=0 ela nao depende de nenhuma tarefa anterior
                if(nextTask.k==0){
                    machines[i].processNextTask(0);
                    numberTasks--;
                    continue;
                }

                Task previousTask = getPreviousTask(nextTask);
                int machinePreviousTask = previousTask.machine;
                int timeTaskEnd = machines[machinePreviousTask].getTimeTaskEnd(previousTask);

                //Se a Task ainda nao foi processada
                if(timeTaskEnd==-1){
                    continue;
                }
                //Se ela ja foi:
                int nextTaskMachine = nextTask.machine;
                if(timeTaskEnd<(machines[nextTaskMachine].getCurrentTime())){  // Se a task anterior terminou antes do currentTime da maquina responsavel pela proxima task
                    machines[nextTaskMachine].processNextTask(0);               // Nao precisa esperar
                    //cout << "Adicionei uma Task sem wait" << endl;
                } else{
                    int wait =  timeTaskEnd - machines[nextTaskMachine].getCurrentTime(); // Calcula o tempo de espera
                    machines[nextTaskMachine].processNextTask(wait);
                    //printf("Preciso esperar a task {%d, %d} ficar pronta na maquina %d, a maquina %d esta no tempo %d, logo ficarei esperando %d\n", previousTask.job, previousTask.k, previousTask.machine, nextTaskMachine,machines[nextTaskMachine].getCurrentTime(),   timeTaskEnd);
                }
                
                numberTasks--;
            }

        }
    }

    

};