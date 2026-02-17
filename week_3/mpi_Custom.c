// The strict signature required by MPI
void my_custom_sum(void *invec, void *inoutvec, int *len, MPI_Datatype *datatype) {
    int *in = (int *)invec;
    int *inout = (int *)inoutvec;
    
    for (int i = 0; i < *len; i++) {
        inout[i] = in[i] + inout[i]; // Custom logic goes here
    }
}
