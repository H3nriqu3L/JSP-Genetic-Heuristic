#include <queue>
#include <utility>
#include <fstream>
using namespace std;

struct Task
{

    int k;      // job k na fila, precisa que todos os k-1 tenham sido processados para poder ser processado
    int machine;    // Maquina que deve ser executado
    int time;   // tempo de duracao da execucao
    int job; // numero job
    
    Task() = default;  // Construtor padrão

    Task(int kValue, int machineValue, int timeValue, int jobValue)
        : k(kValue), machine(machineValue), time(timeValue), job(jobValue) {
    }
    
    bool operator==(const Task& other) const {
        return (k == other.k && machine == other.machine && time == other.time && job == other.job);
    }
};

struct Schedule{
    int job;
    Task task;


};



class Machine{
    private:
        int m_number; // Machine number
        queue<Task> taskQueue; // fila de tarefas
        vector<pair<Task,int>> processedTasks;
        int currentJobTime;         // tempo atual da máquina
        bool firstTaskProcessed = false;


    public:
        Machine() = default;
        Machine(const queue<Task>& tasks, const int m_n) : taskQueue(tasks), currentJobTime(0), m_number(m_n){} // construtor recebe sua fila de tasks

        void processNextTask(int waitTime){
            if(taskQueue.empty()) return;
            Task nextTask = taskQueue.front();
            
            int durantionTask = nextTask.time + waitTime;
           
              

            updateCurrentTime(durantionTask);
            pair<Task,int> aux(nextTask,getCurrentTime());  // Salva a Task e o tempo em que ela terminara de ser processada
            processedTasks.push_back(aux);
            taskQueue.pop();    //Terminou processar a task
            firstTaskProcessed = true;

        }

        void addTask(const Task& task) {
            taskQueue.push(task);
        }

        Task getNextTask() {
            Task nextTask = taskQueue.front();
            return nextTask;
        }

        int getCurrentTime(){
            return currentJobTime;
        }

        void updateCurrentTime(int time) {
            if(firstTaskProcessed)
                currentJobTime += time+1; //+1 pois se uma task termina no tempo 50 a outra so pode comecar no tempo 51
            else
                currentJobTime += time; //Se nao processamos nenhuma task ainda comecamos do tempo 0
        }

        bool isTaskProcessed(Task task){
            for(int i=0; i<processedTasks.size(); i++){
                if(processedTasks[i].first==task){
                    return true;
                }
            }
            return false;
        }

        int getTimeTaskEnd(Task task){
            if (isTaskProcessed(task)==false) return -1;
            int value;
            for (const auto& pair : processedTasks) {
                Task taskaux = pair.first;
                value = pair.second;

                // Verifica se a tarefa é igual a task
                if (taskaux == task) {
                    //printf("A task {%d, %d} ja foi concluida na maquina %d no tempo %d mas eu estou no tempo %d\n", taskaux.job, taskaux.k, taskaux.machine, value, currentJobTime);
                    return value;
                }
            }

            return -1;
        }

        void printProcessedTasks(){
            cout << "Processamento da Maquina " <<m_number << endl;

            for(int i=0; i<processedTasks.size(); i++){
                printf("Task {job, k, machine, timeStartProcess, timeEndProcess}: {%d, %d, %d} foi processada de %d ate %d\n", processedTasks[i].first.job,processedTasks[i].first.k, processedTasks[i].first.machine, processedTasks[i].second- processedTasks[i].first.time, processedTasks[i].second);
            }
            cout << endl;
        }

        void saveProcessedTasks(std::ofstream& arquivo) {
            for (int i = 0; i < processedTasks.size(); i++) {
                arquivo << processedTasks[i].first.job << " " << processedTasks[i].first.k << " "
                        << processedTasks[i].first.machine << " " << processedTasks[i].second - processedTasks[i].first.time << " "
                        << processedTasks[i].second << std::endl;
            }
        }       

        bool isProcessDone(){
            if(taskQueue.empty()) return true;
            return false;
        }




};