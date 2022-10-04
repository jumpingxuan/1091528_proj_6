#include <iostream>
#include <mutex>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <cctype>
#include <queue>
#include <vector>
using namespace std;
#define pp pair<int,int> //<producer生產的空拍機數量 , producer id>

struct thdata{
	int component[3]; //aircraft, battery, propeller ; dispatcher也會用這個陣列去紀錄各個模組各自提供了多少個
	
	int total; //空拍機的數量(Producer) ; 總共準備多少個模組配件（dispatcher)
	int id; //thread id 1 or 2 or 3
};

int seedx;
string c[3]={"aircraft","battery","propeller"};
//------------------ SHARED DATA ----------------------

//dispatcher會不斷供應三種模組配件到供應台,隨機供應某一種配件,但不會重複
int desk[3]; //desk: 紀錄桌面上還有哪些元件
int desk_t=0;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

int totalDispatcherProduce=0;
int counter=0; // 紀錄現在已經製作多少個空拍機
//------------------ SHARED DATA -----------------------
/*
critical section 是用在update共用變數時,需要使用的一個機制.
藉由mutex lock去將critical section分出來.

*/	

/*    想法區
	用一個void就好,因為dispatcher在每個thread去偵測也不會多放或少放元件到桌上,因為是使用同一個desk去偵測
*/

void* forDispatcher(void* ptr)
{
	
	thdata *datas;
	datas = (thdata *)ptr;
	
	
	while(counter<50)
	{
		
		pthread_mutex_lock(&mutex2);//成功使用mutex處理同步
		
		//dispatcher
		if(datas->id==0&&desk_t<3) 
		{
		
			int check[3]={}; //紀錄這個數字是否被產生過了
			int num=0;//紀錄已經有更新多少元件了，當更新次數>=3，一定要跳出迴圈
	
			while(true){ 
				
				//seedx決定亂數決定哪個元件
				seedx=rand()%3; //隨機決定要放哪一個元件到桌上
				num++;
				if(check[seedx]==0) //這個亂數還沒產生過
				{	
					
					check[seedx]=1;
					
					if(desk[seedx]==0)//桌上沒有這個元件
					{
						pthread_mutex_lock(&mutex1); //成功使用mutex機制處理配件供應
						sleep(1);
						//當已經生產到50個了 就不要再繼續做了
						if(counter==50){
							pthread_mutex_unlock(&mutex1);
							break;
						}
						
						
						desk[seedx]=1;
						desk_t++;
						
						totalDispatcherProduce++;
						cout<<"Dispatcher: "<<c[seedx]<<endl;
						pthread_mutex_unlock(&mutex1);
						
						break;
						
					}
					
				}				
				else if(num==3){
					break;
				}
				
			}
		
		}	
		
	
		
		//producer1 已經有aircraft
		if(datas->id==1&&desk_t>0)	
		{
			
			for(int i=1;i<3;i++){ //producer1有的一個零件了，所以可以從第二個零件開始檢查有沒有
				if(datas->component[i]==0){//producer1沒有這個零件
					if(desk[i]==1){ //桌上有這個零件
						datas->component[i]=1; 
						cout<<"Producer 1 (aircraft): get "<<c[i]<<endl;
						pthread_mutex_lock(&mutex1); //成功使用mutex處理拿取的情況
						sleep(1);
						//當已經生產到50個了 就不要再繼續做了
						if(counter==50){
							pthread_mutex_unlock(&mutex1);
							break;
						}
						desk[i]=0;
						desk_t--;
						//檢查三個元件是否已經都有了，有了就可以製造出一台空拍機
						if(datas->component[1]==1&&datas->component[2]==1){
							counter++;
							datas->total++;
							datas->component[1]=0;
							datas->component[2]=0;
							cout<<"Producer 1 (aircraft): OK, "<<datas->total<<" "<< "drone(s)"<<endl;
						}
			
						pthread_mutex_unlock(&mutex1);
						
						break;
					
					}
				}
			}
		
		}
		//producer2
		else if(datas->id==2&&desk_t>0)	
		{
		
			for(int i=0;i<3;i++){
				if(datas->component[i]==0){//producer2沒有這個零件
					if(desk[i]==1){ //桌上有這個零件
						datas->component[i]=1; 
						cout<<"Producer 2: get "<<c[i]<<endl;
						pthread_mutex_lock(&mutex1); //成功使用mutex處理拿取的情況
						sleep(1);
						//當已經生產到50個了 就不要再繼續做了
						if(counter==50){
							pthread_mutex_unlock(&mutex1);
							break;
						}
						
						desk[i]=0;
						desk_t--;
						//檢查三個元件是否已經都有了，有了就可以製造出一台空拍機
						if(datas->component[0]==1&&datas->component[1]==1&&datas->component[2]==1){
							counter++;
							datas->total++;
							datas->component[1]=0;
							datas->component[2]=0;
							datas->component[0]=0;
							cout<<"Producer 2: OK, "<<datas->total<<" "<<" drone(s)"<<endl;
						}
						pthread_mutex_unlock(&mutex1);
						
						break;
					}
				}
			}		
			
		}
		//producer3
		else if(datas->id==3&&desk_t>0)	
		{
			
			
			for(int i=0;i<3;i++){
			
				if(datas->component[i]==0){//producer3沒有這個零件
					if(desk[i]==1){ //桌上有這個零件
						datas->component[i]=1; 
						cout<<"Producer 3: get "<<c[i]<<endl;
						pthread_mutex_lock(&mutex1);//成功使用mutex處理拿取的情況
						sleep(1);	
						//當已經生產到50個了 就不要再繼續做了
						if(counter==50){
							pthread_mutex_unlock(&mutex1);
							break;
						}				
						desk[i]=0;
						desk_t--;
						//檢查三個元件是否已經都有了，有了就可以製造出一台空拍機
						if(datas->component[0]==1&&datas->component[1]==1&&datas->component[2]==1){
					
							counter++;
							datas->total++;
							datas->component[1]=0;
							datas->component[2]=0;
							datas->component[0]=0;
							cout<<"Producer 3: OK, "<<datas->total<<" "<< "drone(s)"<<endl;
						}
						pthread_mutex_unlock(&mutex1);
						
						break;
						
					}
				}
			}
	
			
		}
		pthread_mutex_unlock(&mutex2);
	}
	
	pthread_exit(NULL);
	
}
int main(int argc,char* argv[])
{
	//從命令列讀入所有整數, 並處理輸入錯誤的情況 10分	
	string check[2];
	
	//輸入引數只能兩個
	if(argc!=3){
		cout<<"Your input is wrong!"<<endl;
		return 0;
	}
	//查看輸出內容是否是數字
	check[0]=argv[1];
	check[1]=argv[2];
	
	for(int i=0;i<check[0].length();i++){
		if(!isdigit(check[0][i])){
			cout<<"input should be digit!"<<endl;
			return 0;
		}
	}
	for(int i=0;i<check[1].length();i++){
		if(!isdigit(check[1][i])){
			cout<<"input should be digit!"<<endl;
			return 0;
		}
	}
	
	//數字範圍檢查
	if(atoi(argv[2])<0||atoi(argv[2])>100){
		cout<<"Your input is out of range!"<<endl;
		return 0;
	}	
	
	//程式開始執行		
	if(atoi(argv[1])==0){ //只做基本功能
		//產生亂數種子,用在dispatcher隨機選擇元件的時候
		srand(atoi(argv[2]));
		
		
		//初始化mutex
		pthread_mutex_init(&mutex1,NULL);
		pthread_mutex_init(&mutex2,NULL);
		
		//建立thread
		pthread_t th[4]; //每個producer 與 dispatcher都有各自的work thread
		//每個thread的data
		thdata data[4];
		desk_t=0;
		//initial dispatcher data
		data[0].total=0;
		data[0].id=0;
		data[0].component[0]=1;
		for(int i=1;i<3;i++){
			data[0].component[i]=0;
		}
		
		//成功建立producers and dispatcher的work threads (20 points)
		pthread_create(&th[0],NULL, forDispatcher, (void *) &data[0]); //dispatcher thread
		sleep(1);
		
		for(int i=1;i<4;i++){
			data[i].total=0;
			data[i].id=i;
			for(int j=0;j<3;j++){
				data[i].component[j]=0;
			}
			pthread_create(&th[i],NULL, forDispatcher, (void *) &data[i]);
			
			//sleep(1);
		}
		
		
		for(int i=0;i<4;i++){
			pthread_join(th[i],NULL);
		}
		
		//由生產量多到少輸出producer各自的生產量
		priority_queue<pp> q;
		for(int i=1;i<4;i++){
			q.push({data[i].total,i});
		}
		
		//在main thread印出結果
		cout<<"Dispatcher totally produced "<<totalDispatcherProduce<<" components"<<endl;
		
		while(!q.empty()){
			cout<<"Producer "<<q.top().second<<": totally produced "<<q.top().first<<endl;
			q.pop();
		}
		
		
		pthread_mutex_destroy(&mutex1);
		pthread_mutex_destroy(&mutex2);
	
		return 0;
	}

}


