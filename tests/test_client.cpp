#include "../core/BranchOffice.h"

int main()
{
    BranchOffice branchOffice("127.0.0.1", 8080);

    // Request a task from the LogisticsCenter
    branchOffice.requestTask();

    // Simulate task progress
    branchOffice.updateTaskStatus("12345", "in_progress");
    branchOffice.updateTaskStatus("12345", "completed");

    return 0;
}
