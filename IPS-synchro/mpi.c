#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int buf[2];
    int cislo, num_procs;

    MPI_Init(&argc, &argv);

    /* Kdo jsem */
    MPI_Comm_rank(MPI_COMM_WORLD, &cislo);

    /* Celkovy pocet procesu */
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	if (cislo == 0) {
		printf("Jsem cislo 0, spusteno %i procesu\n",num_procs);
		sleep(10);
		for(int i=1;i<num_procs;i++) {
			buf[0]=i;
			buf[1]=(2*i);
			MPI_Send(buf, 2, MPI_INT, i,
                     		0, MPI_COMM_WORLD);

		}
		for(int i=1;i<num_procs;i++) {
			/*MPI_Recv(buf, 2, MPI_INT, MPI_ANY_SOURCE,
                     		MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);*/
			MPI_Recv(buf, 2, MPI_INT, i,
                     		MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("prijato %i %i\n",buf[0],buf[1]);
			
		}
		
	} else {
		printf("Jsem cislo %i\n", cislo);
		/*MPI_Recv(buf, 2, MPI_INT, 0,
                     0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);*/
		int xx=0;
		while(!xx) {
			printf("Proc %i: Waiting for message\n",cislo);
			usleep(10);
			MPI_Iprobe(0,MPI_ANY_TAG,  MPI_COMM_WORLD, &xx, MPI_STATUS_IGNORE);
		}
		MPI_Recv(buf, 2, MPI_INT, MPI_ANY_SOURCE,
                     MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		printf("Obdrzel jsem: %i %i\n",buf[0],buf[1]);
		buf[1]=211*cislo;
		MPI_Send(buf, 2, MPI_INT, 0,
                     		0, MPI_COMM_WORLD);
	}
    MPI_Finalize();
    return 0;

}
