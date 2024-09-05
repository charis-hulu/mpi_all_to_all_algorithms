#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
    for(i=0; i<nprocs; i++) {
        send_buffer[i] = (rank * 10) + i; 
    }

    // print_buffer(send_buffer, nprocs, rank);
    // MPI_Barrier(MPI_COMM_WORLD);

    // INITIAL ROTATION
    for(i = 0; i < nprocs; i++) {
       recv_buffer[i] = send_buffer[(rank + i) % nprocs]; 
    }

    // GLOBAL COMMUNICATION
    bool *is_sent_id = malloc(nprocs * sizeof(*is_sent_id));
    memset(is_sent_id, 0, nprocs * sizeof(*is_sent_id));
    MPI_Request requests[2];
    for(int k = 1; k < nprocs; k<<=1) {
        for(i = 1; i < nprocs; i++) {
            if(i & k) {
                is_sent_id[i] = 1;
            }
            else {
                is_sent_id[i] = 0;
            }
        }
        int recvp = (rank - k + nprocs) % nprocs;
        int sendp = (rank + k) % nprocs;
        
        MPI_Irecv(send_buffer, nprocs, MPI_INT, recvp, 0, MPI_COMM_WORLD, &requests[0]);
        // printf("P%d recv from P%d\n", rank, recvp);
        MPI_Isend(recv_buffer, nprocs, MPI_INT, sendp, 0, MPI_COMM_WORLD, &requests[1]);
        // printf("P%d send to P%d\n", rank, sendp);
        MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);

        for(i = 1; i < nprocs; i++) {
            if(is_sent_id[i] == 1) {
                recv_buffer[i] = send_buffer[i];
            }
        }
    }

    // LOCAL INVERSE SHIFT
    for(i = 0; i < nprocs; i++) {
        send_buffer[i] = recv_buffer[i];
    }
    for(i = 0; i < nprocs; i++) {
        recv_buffer[i] = send_buffer[(rank - i + nprocs) % nprocs];
    }
    

    print_buffer(recv_buffer, nprocs, rank);

    MPI_Finalize();
    return 0;
}