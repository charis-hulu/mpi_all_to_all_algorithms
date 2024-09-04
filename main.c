#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void print_buffer(int *buffer, int size, int rank) {
    printf("P%d: ", rank);
    for(int i=0; i<size; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    // Prepare buffers 
    int *send_buffer = malloc(nprocs * sizeof(*send_buffer));
    int *recv_buffer = malloc(nprocs * sizeof(*recv_buffer));
    
    // Initialize data
    int i;
    for(int i=0; i<nprocs; i++) {
        send_buffer[i] = (rank * 10) + i; 
    }

    // print_buffer(send_buffer, nprocs, rank);


    MPI_Request requests[2 * nprocs];
    
    int src;
    for(i = 0; i < nprocs; i++) {
        src = (rank + i) % nprocs;
        MPI_Irecv(&recv_buffer[src], 1, MPI_INT, src, 0, MPI_COMM_WORLD, &requests[i]);
        // printf("P%d receives data from P%d\n", rank, src);
    }

    int des;
    for(i = 0; i < nprocs; i++) {
        des = (rank - i + nprocs) % nprocs;
        // printf("P%d sends data to P%d\n", rank, des);
        MPI_Isend(&send_buffer[des], 1, MPI_INT, des, 0, MPI_COMM_WORLD, &requests[nprocs + i]);
    }

    MPI_Waitall(2 * nprocs, requests, MPI_STATUSES_IGNORE);
    print_buffer(recv_buffer, nprocs, rank);

    MPI_Finalize();
    return 0;
}