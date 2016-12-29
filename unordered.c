
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


#define HELLO_PORT 12345
#define HELLO_GROUP "228.0.0.5"
#define MSGBUFSIZE 256

struct Message {

        char message[50];
        int id;
};

int msg_id; //randomly generated identifier for each node

int x=0;// number of nodes

void *sender(void *a) 
{
	struct sockaddr_in addr;
	struct Message msg;
	int fd, cnt;
	struct ip_mreq mreq;
	//char *message="destroy";
	//printf("enter sender\n");
	//u_char loopch;
	time_t t;
	
	srand((unsigned) time(&t));
	msg_id=rand()%50;
	printf("my id is %d\n\n", msg_id );
	sleep(3); // time to start all nodes
	
	/*create socket*/
    	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0)
     	{
      		perror("socket");
      		exit(1);
     	}


    	/* set up destination address */
    	memset(&addr,0,sizeof(addr));
     	addr.sin_family=AF_INET;
     	addr.sin_addr.s_addr=inet_addr(HELLO_GROUP);
     	addr.sin_port=htons(HELLO_PORT);

     	msg.id=msg_id;
     
     	strcpy(msg.message,"random message:");
     
     	/* now just sendto() our destination! */
     	while (1) 
	{
      		if (sendto(fd,&msg,sizeof(struct Message),0,(struct sockaddr *) &addr,
             	sizeof(addr)) < 0)
		{
           		perror("sendto");
        		exit(1);
      		}
       	sleep(2);
	}


}

void *rcver(void *a)
 {


	struct sockaddr_in addr;
    	struct Message my_msg;
	int fd, nbytes,addrlen;
    	struct ip_mreq mreq;
    	//char msgbuf[MSGBUFSIZE];
    	u_int yes=1;
    	//u_char loopch;
    	int n=0, all_ids[100],j=0,flag=0;




    	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
      		perror("socket");
      		exit(1);
     	}

    	/* allow multiple sockets to use the same ADD and PORT number */
    	if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0)
	{
       		perror("Reusing ADDR failed");
       		exit(1);
       	}
    	if (setsockopt(fd,SOL_SOCKET,SO_REUSEPORT,&yes,sizeof(yes)) < 0) 
	{
       		perror("Reusing ADDR failed");
       		exit(1);
       	}
	
    	/*if (setsockopt(fd,IPPROTO_IP,IP_MULTICAST_LOOP,&loopch,sizeof(loopch)) < 0) 
	{
       		perror("disabling multicast loopback failed");
     		exit(1);
     	 }*/
	//set this opt only when you test on different machines.
	
     	/* set up destination address */
     	memset(&addr,0,sizeof(addr));
     	addr.sin_family=AF_INET;
     	addr.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
    	addr.sin_port=htons(HELLO_PORT);
     
     	/* bind to receive address */
     	if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) 
	{
      		perror("bind");
      		exit(1);
     	}
     
     	/* use setsockopt() to request that the kernel join a multicast group */
     	mreq.imr_multiaddr.s_addr=inet_addr(HELLO_GROUP);
     	mreq.imr_interface.s_addr=htons(INADDR_ANY);
     	if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) 
	{
      		perror("setsockopt");
      		exit(1);
     	}

     	all_ids[n]=msg_id;
     	n++;
     	/* now just enter a read-print loop */
      	while(1)
    	{
      		addrlen=sizeof(addr);
      		if ((nbytes=recvfrom(fd,&my_msg,sizeof(struct Message),0,(struct sockaddr *) &addr,&addrlen)) < 0) 
      		{
           		perror("recvfrom");
           		exit(1);
      		}
		//LINEAR SEARCH TO CHECK IF THE MESSAGE HAS ALREADY BEEN RECIEVED
      		for(j=0;j<n;j++)
      		{
        		if(my_msg.id==all_ids[j])
        		{
            			flag=0;
            			break;
        		}
    		}
    		if(j==n)
    		{
        		flag=1;
    		}


        	if(flag==1)
        	{
            		all_ids[n]=my_msg.id;
            		n++;
            		printf("%s %d\n", my_msg.message, my_msg.id  );
            		if(n==x)
            		{
               			goto EndTransmission;
            		}

        	}

      		sleep(1);
     	}
     	EndTransmission:printf("ending of recieving messages\n");
}



int main(int argc, char *argv[]) 
{
     
     
     
     int err;
     pthread_t thread_s,thread_r;
     
     if(argc != 2)
    {
        printf("\n Usage: %s <no of nodes>  \n",argv[0]);
        return 1;
    } 

    x= atoi(argv[1]);


     //Sender Thread
     pthread_create(&thread_s, NULL, sender, NULL);
     

	//Receiver Thread
     pthread_create(&thread_r, NULL, rcver, NULL);

     pthread_join(thread_s, NULL);
     pthread_join(thread_r, NULL);
     

     return 0;
}
