
#line 1 "neko.cc"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include <list>


#line 1 "../COST/cost.h"

























#ifndef queue_t
#define queue_t SimpleQueue
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <deque>
#include <vector>
#include <assert.h>


#line 1 "../COST/priority_q.h"























#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H
#include <stdio.h>
#include <string.h>














template < class ITEM >
class SimpleQueue 
{
 public:
  SimpleQueue() :m_head(NULL) {};
  void EnQueue(ITEM*);
  ITEM* DeQueue();
  void Delete(ITEM*);
  ITEM* NextEvent() const { return m_head; };
  const char* GetName();
 protected:
  ITEM* m_head;
};

template <class ITEM>
const char* SimpleQueue<ITEM>::GetName()
{
  static const char* name = "SimpleQueue";
  return name;
}

template <class ITEM>
void SimpleQueue<ITEM>::EnQueue(ITEM* item)
{
  if( m_head==NULL || item->time < m_head->time )
  {
    if(m_head!=NULL)m_head->prev=item;
    item->next=m_head;
    m_head=item;
    item->prev=NULL;
    return;
  }
    
  ITEM* i=m_head;
  while( i->next!=NULL && item->time > i->next->time)
    i=i->next;
  item->next=i->next;
  if(i->next!=NULL)i->next->prev=item;
  i->next=item;
  item->prev=i;

}

template <class ITEM>
ITEM* SimpleQueue<ITEM> ::DeQueue()
{
  if(m_head==NULL)return NULL;
  ITEM* item = m_head;
  m_head=m_head->next;
  if(m_head!=NULL)m_head->prev=NULL;
  return item;
}

template <class ITEM>
void SimpleQueue<ITEM>::Delete(ITEM* item)
{
  if(item==NULL) return;

  if(item==m_head)
  {
    m_head=m_head->next;
    if(m_head!=NULL)m_head->prev=NULL;
  }
  else
  {
    item->prev->next=item->next;
    if(item->next!=NULL)
      item->next->prev=item->prev;
  }

}

template <class ITEM>
class GuardedQueue : public SimpleQueue<ITEM>
{
 public:
  void Delete(ITEM*);
  void EnQueue(ITEM*);
  bool Validate(const char*);
};
template <class ITEM>
void GuardedQueue<ITEM>::EnQueue(ITEM* item)
{

  ITEM* i=SimpleQueue<ITEM>::m_head;
  while(i!=NULL)
  {
    if(i==item)
    {
      pthread_printf("queue error: item %f(%p) is already in the queue\n",item->time,item);
    }
    i=i->next;
  }
  SimpleQueue<ITEM>::EnQueue(item);
}

template <class ITEM>
void GuardedQueue<ITEM>::Delete(ITEM* item)
{
  ITEM* i=SimpleQueue<ITEM>::m_head;
  while(i!=item&&i!=NULL)
    i=i->next;
  if(i==NULL)
    pthread_printf("error: cannot find the to-be-deleted event %f(%p)\n",item->time,item);
  else
    SimpleQueue<ITEM>::Delete(item);
}

template <class ITEM>
bool GuardedQueue<ITEM>::Validate(const char* s)
{
  char out[1000],buff[100];

  ITEM* i=SimpleQueue<ITEM>::m_head;
  bool qerror=false;

  sprintf(out,"queue error %s : ",s);
  while(i!=NULL)
  {
    sprintf(buff,"%f ",i->time);
    strcat(out,buff);
    if(i->next!=NULL)
      if(i->next->prev!=i)
      {
	qerror=true;
	sprintf(buff," {broken} ");
	strcat(out,buff);
      }
    if(i==i->next)
    {
      qerror=true;
      sprintf(buff,"{loop}");
      strcat(out,buff);
      break;
    }
    i=i->next;
  }
  if(qerror)
    printf("%s\n",out);
  return qerror;
}

template <class ITEM>
class ErrorQueue : public SimpleQueue<ITEM>
{
 public:
  ITEM* DeQueue(double);
  const char* GetName();
};

template <class ITEM>
const char* ErrorQueue<ITEM>::GetName()
{
  static const char* name = "ErrorQueue";
  return name;
}

template <class ITEM>
ITEM* ErrorQueue<ITEM> ::DeQueue(double stoptime)
{
  

  if(drand48()>0.5)
    return SimpleQueue<ITEM>::DeQueue();

  int s=0;
  ITEM* e;
  e=SimpleQueue<ITEM>::m_head;
  while(e!=NULL&&e->time<stoptime)
  {
    s++;
    e=e->next;
  }
  e=SimpleQueue<ITEM>::m_head;
  s=(int)(s*drand48());
  while(s!=0)
  {
    e=e->next;
    s--;
  }
  Delete(e);
  return e;
}

template < class ITEM >
class HeapQueue 
{
 public:
  HeapQueue();
  ~HeapQueue();
  void EnQueue(ITEM*);
  ITEM* DeQueue();
  void Delete(ITEM*);
  const char* GetName();
  ITEM* NextEvent() const { return num_of_elems?elems[0]:NULL; };
 private:
  void SiftDown(int);
  void PercolateUp(int);
  void Validate(const char*);
        
  ITEM** elems;
  int num_of_elems;
  int curr_max;
};

template <class ITEM>
const char* HeapQueue<ITEM>::GetName()
{
  static const char* name = "HeapQueue";
  return name;
}

template <class ITEM>
void HeapQueue<ITEM>::Validate(const char* s)
{
  int i,j;
  char out[1000],buff[100];
  for(i=0;i<num_of_elems;i++)
    if(  ((2*i+1)<num_of_elems&&elems[i]->time>elems[2*i+1]->time) ||
	 ((2*i+2)<num_of_elems&&elems[i]->time>elems[2*i+2]->time) )
    {
      sprintf(out,"queue error %s : ",s);
      for(j=0;j<num_of_elems;j++)
      {
	if(i!=j)
	  sprintf(buff,"%f(%d) ",elems[j]->time,j);
	else
	  sprintf(buff,"{%f(%d)} ",elems[j]->time,j);
	strcat(out,buff);
      }
      printf("%s\n",out);
    }
}
template <class ITEM>
HeapQueue<ITEM>::HeapQueue()
{
  curr_max=16;
  elems=new ITEM*[curr_max];
  num_of_elems=0;
}
template <class ITEM>
HeapQueue<ITEM>::~HeapQueue()
{
  delete [] elems;
}
template <class ITEM>
void HeapQueue<ITEM>::SiftDown(int node)
{
  if(num_of_elems<=1) return;
  int i=node,k,c1,c2;
  ITEM* temp;
        
  do{
    k=i;
    c1=c2=2*i+1;
    c2++;
    if(c1<num_of_elems && elems[c1]->time < elems[i]->time)
      i=c1;
    if(c2<num_of_elems && elems[c2]->time < elems[i]->time)
      i=c2;
    if(k!=i)
    {
      temp=elems[i];
      elems[i]=elems[k];
      elems[k]=temp;
      elems[k]->pos=k;
      elems[i]->pos=i;
    }
  }while(k!=i);
}
template <class ITEM>
void HeapQueue<ITEM>::PercolateUp(int node)
{
  int i=node,k,p;
  ITEM* temp;
        
  do{
    k=i;
    if( (p=(i+1)/2) != 0)
    {
      --p;
      if(elems[i]->time < elems[p]->time)
      {
	i=p;
	temp=elems[i];
	elems[i]=elems[k];
	elems[k]=temp;
	elems[k]->pos=k;
	elems[i]->pos=i;
      }
    }
  }while(k!=i);
}

template <class ITEM>
void HeapQueue<ITEM>::EnQueue(ITEM* item)
{
  if(num_of_elems>=curr_max)
  {
    curr_max*=2;
    ITEM** buffer=new ITEM*[curr_max];
    for(int i=0;i<num_of_elems;i++)
      buffer[i]=elems[i];
    delete[] elems;
    elems=buffer;
  }
        
  elems[num_of_elems]=item;
  elems[num_of_elems]->pos=num_of_elems;
  num_of_elems++;
  PercolateUp(num_of_elems-1);
}

template <class ITEM>
ITEM* HeapQueue<ITEM>::DeQueue()
{
  if(num_of_elems<=0)return NULL;
        
  ITEM* item=elems[0];
  num_of_elems--;
  elems[0]=elems[num_of_elems];
  elems[0]->pos=0;
  SiftDown(0);
  return item;
}

template <class ITEM>
void HeapQueue<ITEM>::Delete(ITEM* item)
{
  int i=item->pos;

  num_of_elems--;
  elems[i]=elems[num_of_elems];
  elems[i]->pos=i;
  SiftDown(i);
  PercolateUp(i);
}



#define CQ_MAX_SAMPLES 25

template <class ITEM>
class CalendarQueue 
{
 public:
  CalendarQueue();
  const char* GetName();
  ~CalendarQueue();
  void enqueue(ITEM*);
  ITEM* dequeue();
  void EnQueue(ITEM*);
  ITEM* DeQueue();
  ITEM* NextEvent() const { return m_head;}
  void Delete(ITEM*);
 private:
  long last_bucket,number_of_buckets;
  double bucket_width;
        
  void ReSize(long);
  double NewWidth();

  ITEM ** buckets;
  long total_number;
  double bucket_top;
  long bottom_threshold;
  long top_threshold;
  double last_priority;
  bool resizable;

  ITEM* m_head;
  char m_name[100];
};


template <class ITEM>
const char* CalendarQueue<ITEM> :: GetName()
{
  sprintf(m_name,"Calendar Queue (bucket width: %.2e, size: %ld) ",
	  bucket_width,number_of_buckets);
  return m_name;
}
template <class ITEM>
CalendarQueue<ITEM>::CalendarQueue()
{
  long i;
        
  number_of_buckets=16;
  bucket_width=1.0;
  bucket_top=bucket_width;
  total_number=0;
  last_bucket=0;
  last_priority=0.0;
  top_threshold=number_of_buckets*2;
  bottom_threshold=number_of_buckets/2-2;
  resizable=true;
        
  buckets= new ITEM*[number_of_buckets];
  for(i=0;i<number_of_buckets;i++)
    buckets[i]=NULL;
  m_head=NULL;

}
template <class ITEM>
CalendarQueue<ITEM>::~CalendarQueue()
{
  delete [] buckets;
}
template <class ITEM>
void CalendarQueue<ITEM>::ReSize(long newsize)
{
  long i;
  ITEM** old_buckets=buckets;
  long old_number=number_of_buckets;
        
  resizable=false;
  bucket_width=NewWidth();
  buckets= new ITEM*[newsize];
  number_of_buckets=newsize;
  for(i=0;i<newsize;i++)
    buckets[i]=NULL;
  last_bucket=0;
  total_number=0;

  
        
  ITEM *item;
  for(i=0;i<old_number;i++)
  {
    while(old_buckets[i]!=NULL)
    {
      item=old_buckets[i];
      old_buckets[i]=item->next;
      enqueue(item);
    }
  }
  resizable=true;
  delete[] old_buckets;
  number_of_buckets=newsize;
  top_threshold=number_of_buckets*2;
  bottom_threshold=number_of_buckets/2-2;
  bucket_top=bucket_width*((long)(last_priority/bucket_width)+1)+bucket_width*0.5;
  last_bucket = long(last_priority/bucket_width) % number_of_buckets;

}
template <class ITEM>
ITEM* CalendarQueue<ITEM>::DeQueue()
{
  ITEM* head=m_head;
  m_head=dequeue();
  return head;
}
template <class ITEM>
ITEM* CalendarQueue<ITEM>::dequeue()
{
  long i;
  for(i=last_bucket;;)
  {
    if(buckets[i]!=NULL&&buckets[i]->time<bucket_top)
    {
      ITEM * item=buckets[i];
      buckets[i]=buckets[i]->next;
      total_number--;
      last_bucket=i;
      last_priority=item->time;
                        
      if(resizable&&total_number<bottom_threshold)
	ReSize(number_of_buckets/2);
      item->next=NULL;
      return item;
    }
    else
    {
      i++;
      if(i==number_of_buckets)i=0;
      bucket_top+=bucket_width;
      if(i==last_bucket)
	break;
    }
  }
        
  
  long smallest;
  for(smallest=0;smallest<number_of_buckets;smallest++)
    if(buckets[smallest]!=NULL)break;

  if(smallest >= number_of_buckets)
  {
    last_priority=bucket_top;
    return NULL;
  }

  for(i=smallest+1;i<number_of_buckets;i++)
  {
    if(buckets[i]==NULL)
      continue;
    else
      if(buckets[i]->time<buckets[smallest]->time)
	smallest=i;
  }
  ITEM * item=buckets[smallest];
  buckets[smallest]=buckets[smallest]->next;
  total_number--;
  last_bucket=smallest;
  last_priority=item->time;
  bucket_top=bucket_width*((long)(last_priority/bucket_width)+1)+bucket_width*0.5;
  item->next=NULL;
  return item;
}
template <class ITEM>
void CalendarQueue<ITEM>::EnQueue(ITEM* item)
{
  
  if(m_head==NULL)
  {
    m_head=item;
    return;
  }
  if(m_head->time>item->time)
  {
    enqueue(m_head);
    m_head=item;
  }
  else
    enqueue(item);
}
template <class ITEM>
void CalendarQueue<ITEM>::enqueue(ITEM* item)
{
  long i;
  if(item->time<last_priority)
  {
    i=(long)(item->time/bucket_width);
    last_priority=item->time;
    bucket_top=bucket_width*(i+1)+bucket_width*0.5;
    i=i%number_of_buckets;
    last_bucket=i;
  }
  else
  {
    i=(long)(item->time/bucket_width);
    i=i%number_of_buckets;
  }

        
  

  if(buckets[i]==NULL||item->time<buckets[i]->time)
  {
    item->next=buckets[i];
    buckets[i]=item;
  }
  else
  {

    ITEM* pos=buckets[i];
    while(pos->next!=NULL&&item->time>pos->next->time)
    {
      pos=pos->next;
    }
    item->next=pos->next;
    pos->next=item;
  }
  total_number++;
  if(resizable&&total_number>top_threshold)
    ReSize(number_of_buckets*2);
}
template <class ITEM>
void CalendarQueue<ITEM>::Delete(ITEM* item)
{
  if(item==m_head)
  {
    m_head=dequeue();
    return;
  }
  long j;
  j=(long)(item->time/bucket_width);
  j=j%number_of_buckets;
        
  

  
  

  ITEM** p = &buckets[j];
  
  ITEM* i=buckets[j];
    
  while(i!=NULL)
  {
    if(i==item)
    { 
      (*p)=item->next;
      total_number--;
      if(resizable&&total_number<bottom_threshold)
	ReSize(number_of_buckets/2);
      return;
    }
    p=&(i->next);
    i=i->next;
  }   
}
template <class ITEM>
double CalendarQueue<ITEM>::NewWidth()
{
  long i, nsamples;
        
  if(total_number<2) return 1.0;
  if(total_number<=5)
    nsamples=total_number;
  else
    nsamples=5+total_number/10;
  if(nsamples>CQ_MAX_SAMPLES) nsamples=CQ_MAX_SAMPLES;
        
  long _last_bucket=last_bucket;
  double _bucket_top=bucket_top;
  double _last_priority=last_priority;
        
  double AVG[CQ_MAX_SAMPLES],avg1=0,avg2=0;
  ITEM* list,*next,*item;
        
  list=dequeue(); 
  long real_samples=0;
  while(real_samples<nsamples)
  {
    item=dequeue();
    if(item==NULL)
    {
      item=list;
      while(item!=NULL)
      {
	next=item->next;
	enqueue(item);
	item=next;      
      }

      last_bucket=_last_bucket;
      bucket_top=_bucket_top;
      last_priority=_last_priority;

                        
      return 1.0;
    }
    AVG[real_samples]=item->time-list->time;
    avg1+=AVG[real_samples];
    if(AVG[real_samples]!=0.0)
      real_samples++;
    item->next=list;
    list=item;
  }
  item=list;
  while(item!=NULL)
  {
    next=item->next;
    enqueue(item);
    item=next;      
  }
        
  last_bucket=_last_bucket;
  bucket_top=_bucket_top;
  last_priority=_last_priority;
        
  avg1=avg1/(double)(real_samples-1);
  avg1=avg1*2.0;
        
  
  long count=0;
  for(i=0;i<real_samples-1;i++)
  {
    if(AVG[i]<avg1&&AVG[i]!=0)
    {
      avg2+=AVG[i];
      count++;
    }
  }
  if(count==0||avg2==0)   return 1.0;
        
  avg2 /= (double) count;
  avg2 *= 3.0;
        
  return avg2;
}

#endif /*PRIORITY_QUEUE_H*/

#line 38 "../COST/cost.h"


#line 1 "../COST/corsa_alloc.h"
































#ifndef corsa_allocator_h
#define corsa_allocator_h

#include <typeinfo>
#include <string>

class CorsaAllocator
{
private:
    struct DT{
#ifdef CORSA_DEBUG
	DT* self;
#endif
	DT* next;
    };
public:
    CorsaAllocator(unsigned int );         
    CorsaAllocator(unsigned int, int);     
    ~CorsaAllocator();		
    void *alloc();		
    void free(void*);
    unsigned int datasize() 
    {
#ifdef CORSA_DEBUG
	return m_datasize-sizeof(DT*);
#else
	return m_datasize; 
#endif
    }
    int size() { return m_size; }
    int capacity() { return m_capacity; }			
    
    const char* GetName() { return m_name.c_str(); }
    void SetName( const char* name) { m_name=name; } 

private:
    CorsaAllocator(const CorsaAllocator& ) {}  
    void Setup(unsigned int,int); 
    void InitSegment(int);
  
    unsigned int m_datasize;
    char** m_segments;	          
    int m_segment_number;         
    int m_segment_max;      
    int m_segment_size;	          
				  
    DT* m_free_list; 
    int m_size;
    int m_capacity;

    int m_free_times,m_alloc_times;
    int m_max_allocs;

    std::string m_name;
};
#ifndef CORSA_NODEF
CorsaAllocator::CorsaAllocator(unsigned int datasize)
{
    Setup(datasize,256);	  
}

CorsaAllocator::CorsaAllocator(unsigned int datasize, int segsize)
{
    Setup(datasize,segsize);
}

CorsaAllocator::~CorsaAllocator()
{
    #ifdef CORSA_DEBUG
    printf("%s -- alloc: %d, free: %d, max: %d\n",GetName(),
	   m_alloc_times,m_free_times,m_max_allocs);
    #endif

    for(int i=0;i<m_segment_number;i++)
	delete[] m_segments[i];	   
    delete[] m_segments;			
}

void CorsaAllocator::Setup(unsigned int datasize,int seg_size)
{

    char buffer[50];
    sprintf(buffer,"%s[%d]",typeid(*this).name(),datasize);
    m_name = buffer;

#ifdef CORSA_DEBUG
    datasize+=sizeof(DT*);  
#endif

    if(datasize<sizeof(DT))datasize=sizeof(DT);
    m_datasize=datasize;
    if(seg_size<16)seg_size=16;    
    m_segment_size=seg_size;			
    m_segment_number=1;		   
    m_segment_max=seg_size;	   
    m_segments= new char* [ m_segment_max ] ;   
    m_segments[0]= new char [m_segment_size*m_datasize];  

    m_size=0;
    m_capacity=0;
    InitSegment(0);

    m_free_times=m_alloc_times=m_max_allocs=00;
}

void CorsaAllocator::InitSegment(int s)
{
    char* p=m_segments[s];
    m_free_list=reinterpret_cast<DT*>(p);
    for(int i=0;i<m_segment_size-1;i++,p+=m_datasize)
    {
	reinterpret_cast<DT*>(p)->next=
	    reinterpret_cast<DT*>(p+m_datasize);
    }
    reinterpret_cast<DT*>(p)->next=NULL;
    m_capacity+=m_segment_size;
}

void* CorsaAllocator::alloc()
{
    #ifdef CORSA_DEBUG
    m_alloc_times++;
    if(m_alloc_times-m_free_times>m_max_allocs)
	m_max_allocs=m_alloc_times-m_free_times;
    #endif
    if(m_free_list==NULL)	
    
    {
	int i;
	if(m_segment_number==m_segment_max)	
	
	
	{
	    m_segment_max*=2;		
	    char** buff;
	    buff=new char* [m_segment_max];   
#ifdef CORSA_DEBUG
	    if(buff==NULL)
	    {
		printf("CorsaAllocator runs out of memeory.\n");
		exit(1);
	    }
#endif
	    for(i=0;i<m_segment_number;i++)
		buff[i]=m_segments[i];	
	    delete [] m_segments;		
	    m_segments=buff;
	}
	m_segment_size*=2;
	m_segments[m_segment_number]=new char[m_segment_size*m_datasize];
#ifdef CORSA_DEBUG
	    if(m_segments[m_segment_number]==NULL)
	    {
		printf("CorsaAllocator runs out of memeory.\n");
		exit(1);
	    }
#endif
	InitSegment(m_segment_number);
	m_segment_number++;
    }

    DT* item=m_free_list;		
    m_free_list=m_free_list->next;
    m_size++;

#ifdef CORSA_DEBUG
    item->self=item;
    char* p=reinterpret_cast<char*>(item);
    p+=sizeof(DT*);
    
    return static_cast<void*>(p);
#else
    return static_cast<void*>(item);
#endif
}

void CorsaAllocator::free(void* data)
{
#ifdef CORSA_DEBUG
    m_free_times++;
    char* p=static_cast<char*>(data);
    p-=sizeof(DT*);
    DT* item=reinterpret_cast<DT*>(p);
    
    if(item!=item->self)
    {
	if(item->self==(DT*)0xabcd1234)
	    printf("%s: packet at %p has already been released\n",GetName(),p+sizeof(DT*)); 
	else
	    printf("%s: %p is probably not a pointer to a packet\n",GetName(),p+sizeof(DT*));
    }
    assert(item==item->self);
    item->self=(DT*)0xabcd1234;
#else
    DT* item=static_cast<DT*>(data);
#endif

    item->next=m_free_list;
    m_free_list=item;
    m_size--;
}
#endif /* CORSA_NODEF */

#endif /* corsa_allocator_h */

#line 39 "../COST/cost.h"


class trigger_t {};
typedef double simtime_t;

#ifdef COST_DEBUG
#define Printf(x) Print x
#else
#define Printf(x)
#endif



class TimerBase;



struct CostEvent
{
  double time;
  CostEvent* next;
  union {
    CostEvent* prev;
    int pos;  
  };
  TimerBase* object;
  int index;
  unsigned char active;
};



class TimerBase
{
 public:
  virtual void activate(CostEvent*) = 0;
  inline virtual ~TimerBase() {}	
};

class TypeII;



class CostSimEng
{
 public:

  class seed_t
      {
       public:
	void operator = (long seed) { srand48(seed); };
      };
  seed_t		Seed;
  CostSimEng()
      : stopTime( 0), clearStatsTime( 0), m_clock( 0.0)
      {
        if( m_instance == NULL)
	  m_instance = this;
        else
	  printf("Error: only one simulation engine can be created\n");
      }
  virtual		~CostSimEng()	{ }
  static CostSimEng	*Instance()
      {
        if(m_instance==NULL)
        {
	  printf("Error: a simulation engine has not been initialized\n");
	  m_instance = new CostSimEng;
        }
        return m_instance;
      }
  CorsaAllocator	*GetAllocator(unsigned int datasize)
      {
    	for(unsigned int i=0;i<m_allocators.size();i++)
    	{
	  if(m_allocators[i]->datasize()==datasize)return m_allocators[i];
    	} 
    	CorsaAllocator* allocator=new CorsaAllocator(datasize);
    	char buffer[25];
    	sprintf(buffer,"EventAllocator[%d]",datasize);
    	allocator->SetName(buffer);
    	m_allocators.push_back(allocator);
    	return allocator;
      }
  void		AddComponent(TypeII*c)
      {
        m_components.push_back(c);
      }
  void		ScheduleEvent(CostEvent*e)
      {
	if( e->time < m_clock)
	  assert(e->time>=m_clock);
        
        m_queue.EnQueue(e);
      }
  void		CancelEvent(CostEvent*e)
      {
        
        m_queue.Delete(e);
      }
  double	Random( double v=1.0)	{ return v*drand48();}
  int		Random( int v)		{ return (int)(v*drand48()); }
  double	Exponential(double mean)	{ return -mean*log(Random());}
  virtual void	Start()		{}
  virtual void	Stop()		{}
  void		Run();
  double	SimTime()	{ return m_clock; } 
  void		StopTime( double t)	{ stopTime = t; }
  double	StopTime() const	{ return stopTime; }
  void		ClearStatsTime( double t)	{ clearStatsTime = t; }
  double	ClearStatsTime() const	{ return clearStatsTime; }
  virtual void	ClearStats()	{}
 private:
  double	stopTime;
  double	clearStatsTime;	
  double	eventRate;
  double	runningTime;
  long		eventsProcessed;
  double	m_clock;
  queue_t<CostEvent>	m_queue;
  std::vector<TypeII*>	m_components;
  static CostSimEng	*m_instance;
  std::vector<CorsaAllocator*>	m_allocators;
};




class TypeII
{
 public: 
  virtual void Start() {};
  virtual void Stop() {};
  inline virtual ~TypeII() {}		
  TypeII()
      {
        m_simeng=CostSimEng::Instance();
        m_simeng->AddComponent(this);
      }

#ifdef COST_DEBUG
  void Print(const bool, const char*, ...);
#endif
    
  double Random(double v=1.0) { return v*drand48();}
  int Random(int v) { return (int)(v*drand48());}
  double Exponential(double mean) { return -mean*log(Random());}
  inline double SimTime() const { return m_simeng->SimTime(); }
  inline double StopTime() const { return m_simeng->StopTime(); }
 private:
  CostSimEng* m_simeng;
}; 

#ifdef COST_DEBUG
void TypeII::Print(const bool flag, const char* format, ...)
{
  if(flag==false) return;
  va_list ap;
  va_start(ap, format);
  printf("[%.10f] ",SimTime());
  vprintf(format,ap);
  va_end(ap);
}
#endif

CostSimEng* CostSimEng::m_instance = NULL;

void CostSimEng::Run()
{
  double	nextTime = (clearStatsTime != 0.0 && clearStatsTime < stopTime) ? clearStatsTime : stopTime;

  m_clock = 0.0;
  eventsProcessed = 0l;
  std::vector<TypeII*>::iterator iter;
      
  struct timeval start_time;    
  gettimeofday( &start_time, NULL);

  Start();

  for( iter = m_components.begin(); iter != m_components.end(); iter++)
    (*iter)->Start();

  CostEvent* e=m_queue.DeQueue();
  while( e != NULL)
  {
    if( e->time >= nextTime)
    {
      if( nextTime == stopTime)
	break;
      
      printf( "Clearing statistics @ %f\n", nextTime);
      nextTime = stopTime;
      ClearStats();
    }
    
    assert( e->time >= m_clock);
    m_clock = e->time;
    e->object->activate( e);
    eventsProcessed++;
    e = m_queue.DeQueue();
  }
  m_clock = stopTime;
  for(iter = m_components.begin(); iter != m_components.end(); iter++)
    (*iter)->Stop();
	    
  Stop();

  struct timeval stop_time;    
  gettimeofday(&stop_time,NULL);

  runningTime = stop_time.tv_sec - start_time.tv_sec +
      (stop_time.tv_usec - start_time.tv_usec) / 1000000.0;
  eventRate = eventsProcessed/runningTime;
  
  
  printf("# -------------------------------------------------------------------------\n");	
  printf("# CostSimEng with %s, stopped at %f\n", m_queue.GetName(), stopTime);	
  printf("# %ld events processed in %.3f seconds, event processing rate: %.0f\n",	
  eventsProcessed, runningTime, eventRate);
  
}







#line 15 "neko.cc"


#line 1 "../Config/constants.h"






const int runTime = 120;           	
const int rnd = 1;                      
const int lowBW = 2;                    
double medBW;                    
const int highBW = 8;                   
const int propagation = 1;              
const int WinTime = 540;     			
const double RSSIth = -75;				
const int maxIntNum = 3;
bool channel_report = false;				
bool stats_report = true;					



std::string policy;







const std::vector<std::vector<double>> Channels{{1,6,11},
																								{38,46,58},
																								{55,71,15}};







const int Lpckt = 12000;                          
const int Lsf = 16;                               
const int Lmac = 320;                             
const int Ltb = 18;                               

const int Lack = 112;															

const int Lrts = 160;                             
const int Lcts = 112;                             
const int CW = 15;                                
const double Tempty = 9*pow(10,-6);               
const double Tsifs = 16*pow(10,-6);               
const double Tdifs = 34*pow(10,-6);               
const double TphyL = 20*pow(10,-6);               
const double TphyHE = 164*pow(10,-6);             
const double Tofdm_leg = 4*pow(10,-6);            
const double Tofdm = 16*pow(10,-6);               
const double legacyRate = 24;                     
const double Pe = 0.1;                            
const int NF = 7;								  								








const double t_EndFlow = 1;            
const double t_ActFlow = 3;            






const int off_DCA = 50;              
const int off_DAPS = 7200;             

#line 16 "neko.cc"


#line 1 "../Methods/frequency.h"

std::string GetBand (double fc){

	std::string band;
	int fc_band = (int)fc;

	switch (fc_band){
		case 2:{band = "2_4GHz";}break;
		case 5:{band = "5GHz";}break;
		case 6:{band = "6GHz";}break;
	}
	return band;
}

std::pair<double,int> GetFromChN(int ChN){

	double fc;
	int ChW;

	if (ChN == 1){fc = 2.412; ChW = 20;}
	else if( ChN == 6){fc = 2.437; ChW = 20;}
	else if( ChN == 11){fc = 2.462; ChW = 20;}
	else if( ChN == 3){fc = 2.422; ChW = 40;}
	else if( ChN == 36){fc = 5.18; ChW = 20;}
	else if( ChN == 38){fc = 5.19; ChW = 40;}
	else if( ChN == 46){fc = 5.23; ChW = 40;}
	else if( ChN == 42){fc = 5.21; ChW = 80;}
	else if( ChN == 58){fc = 5.29; ChW = 80;}
	else if( ChN == 25){fc = 6.065; ChW = 20;}
	else if( ChN == 51){fc = 6.195; ChW = 40;}
	else if( ChN == 55){fc = 6.215; ChW = 80;}
	else if( ChN == 71){fc = 6.295; ChW = 80;}
	else if( ChN == 47){fc = 6.175; ChW = 160;}
	else if( ChN == 15){fc = 6.015; ChW = 160;}

	return std::make_pair(fc,ChW);
}

bool CheckNeighChOverlapp(double fc, double neighFc){

	bool overlap = false;
	int fc_band = (int)fc;

	switch (fc_band){
		
		case 2:{
			if((fc == neighFc) || ((fc == 2.412) && (neighFc == 2.422)) || ((fc == 2.422) && (neighFc == 2.412))){
				overlap = true;
			}
		}break;

		
		case 5:{
			if ((fc == neighFc) || ((fc == 5.18) && (neighFc == 5.19)) || ((fc == 5.19) && (neighFc == 5.18)) || ((fc == 5.18) && (neighFc == 5.21))|| ((fc == 5.21) && (neighFc == 5.18)) ||
																									((fc == 5.19) && (neighFc == 5.21)) || ((fc == 5.21) && (neighFc == 5.19)) || ((fc == 5.23) && (neighFc == 5.21)) || ((fc == 5.21) && (neighFc == 5.23))){
				overlap = true;
			}
		}break;
		
		case 6:{
			if ((fc == neighFc) || ((fc == 6.195) && (neighFc == 6.175)) || ((fc == 6.175) && (neighFc == 6.195)) || ((fc == 6.195) && (neighFc == 6.215)) ||
																							((fc == 6.215) && (neighFc == 6.195)) || ((fc == 6.215) && (neighFc == 6.175)) || ((fc == 6.175) && (neighFc == 6.215))){
				overlap = true;
			}
		}break;
	}
	return overlap;
}

bool CheckChOverlapp(int ch1, int ch2){

	std::pair<double, int> p = GetFromChN(ch1);
	int band = (int)p.first;
	bool overlap = false;

	switch (band){

		case 2:{
			if((ch1 == ch2) || ((ch1 == 1) && (ch2 == 3)) || ((ch1 == 3) && (ch2 == 1))){
				overlap = true;
			}
		}break;

		case 5:{
			if((ch1 == ch2) || ((ch1 == 36) && (ch2 == 38)) || ((ch1 == 38) && (ch2 == 36)) || ((ch1 == 36) && (ch2 == 42)) || ((ch1 == 42) && (ch2 == 36))
																															|| ((ch1 == 38) && (ch2 == 42)) || ((ch1 == 42) && (ch2 == 38)) || ((ch1 == 46) && (ch2 == 42)) || ((ch1 == 42) && (ch2 == 46))){
				overlap = true;
			}
		}break;

		case 6:{
			if((ch1 == ch2) || ((ch1 == 51) && (ch2 == 55)) || ((ch1 == 55) && (ch2 == 51)) || ((ch1 == 51) && (ch2 == 47))
																											|| ((ch1 == 47) && (ch2 == 51)) || ((ch1 == 55) && (ch2 == 47)) || ((ch1 == 47) && (ch2 == 55))){
				overlap = true;
			}
		}break;
	}

	return overlap;
}

int GetChWFromFc (double fc){

	int ChW;
	int fc_band = (int)fc;

	switch (fc_band){
		case 2:{
			if (fc == 2.422){ChW = 40;}
			else{ChW = 20;}
		}break;

		case 5:{
			if (fc == 5.18){ChW = 20;}
			else if ((fc == 5.19) || (fc == 5.23)){ChW = 40;}
			else{ChW = 80;}
		}break;

		case 6:{
			if (fc == 6.065){ChW = 20;}
			else if (fc == 6.195){ChW = 40;}
			else if ((fc == 6.215) || (fc == 6.295)){ChW = 80;}
			else{ChW = 160;}
		}break;
	}
	return ChW;
}

#line 17 "neko.cc"


#line 1 "../Methods/modulation.h"

int GetMCS (double SNR, int protocol, int ChW){

	int MCS;
	switch (protocol) {

		
      	case 1:{
      		switch (ChW){
      			case 20:{
      				if (SNR < 2) {MCS = -1;} 
					else if (SNR >= 2 && SNR < 5) {MCS = 0;}
					else if (SNR >= 5 && SNR < 9) {MCS = 1;}
					else if (SNR >= 9 && SNR < 11) {MCS = 2;}
					else if (SNR >= 11 && SNR < 15) {MCS = 3;}
					else if (SNR >= 15 && SNR < 18) {MCS = 4;}
					else if (SNR >= 18 && SNR < 20) {MCS = 5;}
					else if (SNR >= 20 && SNR < 25) {MCS = 6;}
					else if (SNR >= 25 && SNR < 29) {MCS = 7;}
					else if (SNR >= 29 && SNR < 31) {MCS = 8;}
					else if (SNR >= 31 && SNR < 34) {MCS = 9;}
					else if (SNR >= 34 && SNR < 37) {MCS = 10;}
					else if (SNR >= 37) {MCS = 11;}
      			}break;
      			case 40:{
      				if (SNR < 5) {MCS = -1;} 
					else if (SNR >= 5 && SNR < 8) {MCS = 0;}
					else if (SNR >= 8 && SNR < 12) {MCS = 1;}
					else if (SNR >= 12 && SNR < 14) {MCS = 2;}
					else if (SNR >= 14 && SNR < 18) {MCS = 3;}
					else if (SNR >= 18 && SNR < 21) {MCS = 4;}
					else if (SNR >= 21 && SNR < 23) {MCS = 5;}
					else if (SNR >= 23 && SNR < 28) {MCS = 6;}
					else if (SNR >= 28 && SNR < 32) {MCS = 7;}
					else if (SNR >= 32 && SNR < 34) {MCS = 8;}
					else if (SNR >= 34 && SNR < 37) {MCS = 9;}
					else if (SNR >= 37 && SNR < 40) {MCS = 10;}
					else if (SNR >= 40) {MCS = 11;}
      			}break;
      			case 80:{
      				if (SNR < 8) {MCS = -1;} 
					else if (SNR >= 8 && SNR < 11) {MCS = 0;}
					else if (SNR >= 11 && SNR < 15) {MCS = 1;}
					else if (SNR >= 15 && SNR < 17) {MCS = 2;}
					else if (SNR >= 17 && SNR < 21) {MCS = 3;}
					else if (SNR >= 21 && SNR < 24) {MCS = 4;}
					else if (SNR >= 24 && SNR < 26) {MCS = 5;}
					else if (SNR >= 26 && SNR < 31) {MCS = 6;}
					else if (SNR >= 31 && SNR < 35) {MCS = 7;}
					else if (SNR >= 35 && SNR < 37) {MCS = 8;}
					else if (SNR >= 37 && SNR < 40) {MCS = 9;}
					else if (SNR >= 40 && SNR < 42) {MCS = 10;}
					else if (SNR >= 42) {MCS = 11;}
      			}break;
      		}
      	}break;

      	
      	case 2:{
      		switch (ChW){
      			case 20:{
      				if (SNR < 2) {MCS = -1;} 
					else if (SNR >= 2 && SNR < 5) {MCS = 0;}
					else if (SNR >= 5 && SNR < 9) {MCS = 1;}
					else if (SNR >= 9 && SNR < 11) {MCS = 2;}
					else if (SNR >= 11 && SNR < 15) {MCS = 3;}
					else if (SNR >= 15 && SNR < 18) {MCS = 4;}
					else if (SNR >= 18 && SNR < 20) {MCS = 5;}
					else if (SNR >= 20 && SNR < 25) {MCS = 6;}
					else if (SNR >= 25 && SNR < 29) {MCS = 7;}
					else if (SNR >= 29 && SNR < 31) {MCS = 8;}
					else if (SNR >= 31 && SNR < 34) {MCS = 9;}
					else if (SNR >= 34 && SNR < 37) {MCS = 10;}
					else if (SNR >= 37) {MCS = 11;}
      			}break;
      			case 40:{
      				if (SNR < 5) {MCS = -1;} 
					else if (SNR >= 5 && SNR < 8) {MCS = 0;}
					else if (SNR >= 8 && SNR < 12) {MCS = 1;}
					else if (SNR >= 12 && SNR < 14) {MCS = 2;}
					else if (SNR >= 14 && SNR < 18) {MCS = 3;}
					else if (SNR >= 18 && SNR < 21) {MCS = 4;}
					else if (SNR >= 21 && SNR < 23) {MCS = 5;}
					else if (SNR >= 23 && SNR < 28) {MCS = 6;}
					else if (SNR >= 28 && SNR < 32) {MCS = 7;}
					else if (SNR >= 32 && SNR < 34) {MCS = 8;}
					else if (SNR >= 34 && SNR < 37) {MCS = 9;}
					else if (SNR >= 37 && SNR < 40) {MCS = 10;}
					else if (SNR >= 40) {MCS = 11;}
      			}break;
      			case 80:{
      				if (SNR < 8) {MCS = -1;} 
					else if (SNR >= 8 && SNR < 11) {MCS = 0;}
					else if (SNR >= 11 && SNR < 15) {MCS = 1;}
					else if (SNR >= 15 && SNR < 17) {MCS = 2;}
					else if (SNR >= 17 && SNR < 21) {MCS = 3;}
					else if (SNR >= 21 && SNR < 24) {MCS = 4;}
					else if (SNR >= 24 && SNR < 26) {MCS = 5;}
					else if (SNR >= 26 && SNR < 31) {MCS = 6;}
					else if (SNR >= 31 && SNR < 35) {MCS = 7;}
					else if (SNR >= 35 && SNR < 37) {MCS = 8;}
					else if (SNR >= 37 && SNR < 40) {MCS = 9;}
					else if (SNR >= 40 && SNR < 42) {MCS = 10;}
					else if (SNR >= 42) {MCS = 11;}
      			}break;
      			case 160:{
      				if (SNR < 11) {MCS = -1;} 
					else if (SNR >= 11 && SNR < 14) {MCS = 0;}
					else if (SNR >= 14 && SNR < 18) {MCS = 1;}
					else if (SNR >= 18 && SNR < 20) {MCS = 2;}
					else if (SNR >= 20 && SNR < 24) {MCS = 3;}
					else if (SNR >= 24 && SNR < 27) {MCS = 4;}
					else if (SNR >= 27 && SNR < 29) {MCS = 5;}
					else if (SNR >= 29 && SNR < 34) {MCS = 6;}
					else if (SNR >= 34 && SNR < 38) {MCS = 7;}
					else if (SNR >= 38 && SNR < 40) {MCS = 8;}
					else if (SNR >= 40 && SNR < 42) {MCS = 9;}
					else if (SNR >= 42 && SNR < 44) {MCS = 10;}
					else if (SNR >= 44) {MCS = 11;}
      			}break;
      		}
      	}break;
    }
    return MCS;
}

int GetBitsPerSimbol(int index){

	int BitsSimbol[] = {1,2,2,4,4,6,6,6,8,8,10,10};  
	return BitsSimbol[index];
}

double GetCodingRate(int index){

	double CR [] = {1/double(2),1/double(2),3/double(4),1/double(2),3/double(4),2/double(3),3/double(4),5/double(6),3/double(4),5/double(6),3/double(4),5/double(6)}; 
	return CR[index];
}

int GetSubcarriers(double fc, int width){

	int subcarriers;
	int band = (int)fc;

	switch (band){
		case 2:{
			switch (width){
				case 20:{subcarriers = 234;}break;
				case 40:{subcarriers = 468;}break;
			}
		}break;

		case 5:{
			switch (width){
				case 20:{subcarriers = 234;}break;
				case 40:{subcarriers = 468;}break;
				case 80:{subcarriers = 980;}break;
			}
		}break;

		case 6:{
			switch (width){
				case 20:{subcarriers = 234;}break;
				case 40:{subcarriers = 468;}break;
				case 80:{subcarriers = 980;}break;
				case 160:{subcarriers = 1960;}break;
			}
		}break;
	}
	return subcarriers;
}

#line 18 "neko.cc"


#line 1 "../Classes/structs.h"





struct Position{

	double x;										
	double y;										
	double z;										
};

struct Capabilities{

	int IEEEProtocol;						
	bool Multilink;							
	bool Mlearning;							
};

struct Configuration{

	int nSS;										
	double TxPower;							
	double CCA;									
};






struct WifiSTA{

	int id;												
	Position coord;								
	std::string traffic_type;			
	std::vector<double> fc;				
	std::vector<double> RSSI;			
	std::vector<double> SNR;			
	std::vector<double> TxRate;		
};

struct WifiAP{

	int id;												
	Position coord;								
	std::vector<double> fc;				
	std::vector<double> ChOcc;		
};






struct APStatistics{

	std::vector<std::vector<int>> ChSelection;			
	std::vector<std::vector<double>> ChReward;			
	std::vector<std::vector<double>> ChOcc;					
	std::vector<double> SimT;												
	std::vector<double> AvgDRPerFlow;
};

struct STAStatistics{

	std::vector<int> APSelection;										
	std::vector<std::vector<double>> SatEvo;				
	std::vector<std::vector<double>> SimT;					
	std::vector<double> AvgSatPerFlow;
	std::vector<double> AvgThPerFlow;
};

#line 19 "neko.cc"


#line 1 "../Methods/pathloss.h"


double PropL(double x, double y, double z, double x1, double y1, double z1, double f){

  switch (propagation) {

    
    case 0:{
      int n_walls = 5;                                         
      int n_floors = 2;                                        
      double dBP = 5;                                         
      double dBP_losses, PL, distance, expo;

      distance = sqrt(pow(x-x1, 2)+pow(y-y1, 2)+pow(z-z1, 2));

      if (distance >= dBP){
        dBP_losses = 35*log10(distance/dBP);
      }
      else{
        dBP_losses = 0;
      }

      expo=((distance/n_floors)+2)/((distance/n_floors)+1)-0.46;

      PL = 40.05 + 20*log10(f/2.4) + 20*log10(std::min(distance,dBP)) + dBP_losses + 18.3*pow((distance/n_floors),expo)+ 5*(distance/n_walls);

      return PL;

    }break;

    
    case 1:{

      int n_walls = 4;                                          
      double dBP = 5;                                           
      double dBP_losses, PL, distance;

      distance = sqrt(pow(x-x1, 2)+pow(y-y1, 2)+pow(z-z1, 2));

      if (distance >= dBP){
        dBP_losses = 35*log10(distance/dBP);
      }
      else{
        dBP_losses = 0;
      }

      PL = 40.05 + 20*log10(f/2.4) + 20*log10(std::min(distance,dBP)) + dBP_losses + 7*n_walls;

      return PL;

    }break;

    
    case 2:{



    }break;
  }
}

double Dmax(double sensitivity, double TxPower, double f){

  switch (propagation){
    case 1:{
        int n_walls = 4;                                         
        double dBP = 5;                                         

        double ChW = GetChWFromFc(f);
        double nCh = ChW/20;
        double PtxLineal = pow(10,((TxPower-30)/10));
        double Ptx_dBm = 10*log10(PtxLineal/nCh) + 30;

        double PL = Ptx_dBm - sensitivity;
        double distance = dBP*pow(10,((PL - 40.05 - 20*log10(f/2.4) - 20*log10(dBP) - 7*n_walls)/35));

        return distance;
    }
  }
}

#line 20 "neko.cc"


#line 1 "../Methods/general.h"

double CalculateTxTime(double L, double TxRate){

	double TimeMPDU, Tack, Trts, Tcts, LDBPS_DL, AirTime;

  LDBPS_DL = (TxRate*pow(10,6))*Tofdm;

	TimeMPDU = TphyHE + std::ceil(((Lsf+Lmac+Lpckt+Ltb)/(LDBPS_DL)))*Tofdm;
	Tack = TphyL + std::ceil(((Lsf+Lack+Ltb)/(legacyRate)))*Tofdm_leg;
	Trts = TphyL + std::ceil(((Lsf+Lrts+Ltb)/(legacyRate)))*Tofdm_leg;
	Tcts = TphyL + std::ceil(((Lsf+Lcts+Ltb)/(legacyRate)))*Tofdm_leg;

	AirTime = (std::ceil((L*pow(10,6)/Lpckt))*(1/(1-Pe)))*(((CW/2)*Tempty)+(Trts+Tsifs+Tcts+Tsifs+TimeMPDU+Tsifs+Tack+Tdifs+Tempty));

	return AirTime;
}

double GetFromTxTime(double at, double TxRate){

	double TimeMPDU, Tack, Trts, Tcts, LDBPS_DL, load;

  LDBPS_DL = (TxRate*pow(10,6))*Tofdm;

	TimeMPDU = TphyHE + std::ceil(((Lsf+Lmac+Lpckt+Ltb)/(LDBPS_DL)))*Tofdm;
	Tack = TphyL + std::ceil(((Lsf+Lack+Ltb)/(legacyRate)))*Tofdm_leg;
	Trts = TphyL + std::ceil(((Lsf+Lrts+Ltb)/(legacyRate)))*Tofdm_leg;
	Tcts = TphyL + std::ceil(((Lsf+Lcts+Ltb)/(legacyRate)))*Tofdm_leg;

	load = (at*Lpckt)/((1/(1-Pe))*(((CW/2)*Tempty)+(Trts+Tsifs+Tcts+Tsifs+TimeMPDU+Tsifs+Tack+Tdifs+Tempty))*pow(10,6));
	return load;
}

double CalculateSNR(double RSSI, double ChW){

	double NoiseFloor = -174 + NF  + 10*log10(ChW*1E6);
	double SNR = RSSI - NoiseFloor;

	return SNR;
}

double CalculateRSSI(double Ptx, double fc, double x, double y, double z, double x1, double y1, double z1){

	



	double ChW = GetChWFromFc(fc);
	double nCh = ChW/20;
	double PtxLineal = pow(10,((Ptx-30)/10));
	double Ptx_dBm = 10*log10(PtxLineal/nCh) + 30;

	double RSSI = Ptx_dBm - PropL(x, y, z, x1, y1, z1, fc);

	return RSSI;
}

double CalculateDataRate(double SNR, double fc, double ChW, Capabilities &cap, Configuration &config){

	int MCS_index = GetMCS(SNR, cap.IEEEProtocol, ChW);
	double rate;
	if (MCS_index >= 0){

		int BitsPerSimbol = GetBitsPerSimbol(MCS_index);
		int Subcarriers = GetSubcarriers(fc, ChW);
		double CodingR = GetCodingRate(MCS_index);

		rate = (config.nSS * BitsPerSimbol * CodingR * Subcarriers * 1/Tofdm) /1000000; 
	}
	else{
		rate = 0;
		
	}

	return rate;
}

double GetActReward (int action, std::vector<int> *action_selection, std::vector<double> *received_reward, std::vector<double> *t_action, double simt){

  int size = t_action->size();
  int times = 0;

  double lowerBound = std::max(0.0,simt-WinTime);
  double upperBound = simt;
	double sumReward = 0;
	double avgReward = 0;

  for (int i=0; i<size; i++){
    if ((lowerBound <= t_action->at(i)) && (t_action->at(i) < upperBound)){
      if (action_selection->at(i) == action){
        sumReward += received_reward->at(i);
        times++;
      }
    }
  }

  if (sumReward != 0)
      avgReward = sumReward/times;
  
  return avgReward;
}

#line 21 "neko.cc"


#line 1 "../Learning/strategies.h"




int Egreedy(int num_actions, std::vector<double>* reward_per_arm, std::vector<double>* occupancy, double epsilon, std::vector<int>* times_arm_selected) {

	double rndProbability = ((double) rand() / (RAND_MAX));
	int arm_index;
	double r_value, o_value;

	if (rndProbability < epsilon) {
		arm_index = rand() % num_actions;
		printf("exploring\n");
	}
	else {

		printf("exploiting\n");
		double max_rew = 0;
		double min_occ = 100;
		double th = 0.85;

		for (int i = 0; i < num_actions; i ++){
			r_value = reward_per_arm->at(i);
			o_value = occupancy->at(i);

			if (r_value >= th){
				if(o_value <= min_occ){
					max_rew = r_value;
					min_occ = o_value;
					arm_index = i;
				}
			}
			else{
				if(r_value >= max_rew){
					max_rew = r_value;
					min_occ = o_value;
					arm_index = i;
				}
			}
		}
	}
	times_arm_selected->at(arm_index) = times_arm_selected->at(arm_index) + 1;
	return arm_index;
}



double gaussrand(double mean, double std){

	static double V1, V2, S;
	static int phase = 0;
	double X;

	if(phase == 0) {
		do {
			double U1 = (double)rand() /  RAND_MAX;
			double U2 = (double)rand() /  RAND_MAX;
			V1 = 2*U1 - 1;
			V2 = 2*U2 - 1;
			S = V1 * V1 + V2 * V2;
		}
    while (S >= 1 || S == 0);
		X = (V1 * sqrt(-2 * log(S) / S)) * std + mean;
	}
  else {
		X = (V1 * sqrt(-2 * log(S) / S)) * std + mean;
	}
	phase = 1 - phase;

	return X;
}


int ThompsonSampling (int num_actions, std::vector<double> *estimated_reward, std::vector<double> *times_arm_selected) {

	int action_ix = 0;
  double theta[num_actions] = {0};
	double mean, variance, times;

	for (int i = 0; i < num_actions; i++) {

		times = times_arm_selected->at(i);
		mean = estimated_reward->at(i);
		variance = 1/(1+times);
		theta[i] = gaussrand(mean,variance);
	}

	double max = 0;
	for (int i = 0; i < num_actions; i ++) {
		if(theta[i] > max) {
			max = theta[i];
			action_ix = i;
		}
	}

	return action_ix;
}

#line 22 "neko.cc"


#line 1 "../Classes/flow.cc"




#line 1 "../Classes/flow.h"




class Flow{

private:

	std::string _type;															
	int _sender;																		
	int _destination;																
	double _timestamp;
	double _length;																	
	std::vector<double> _TxTime;										
	std::vector<double> _Fc;												
	std::vector<std::vector<double>> _Sat;


public:
	Flow();
	~Flow();

	
	void setType(std::string);
	void setSender(int);
	void setDestination(int);
	void setTimeStamp(double);
	void setTxTime(double);
	void setLength(double);
	void setFc(double);
	void setSat(std::vector<double> &);

	
	std::string getType();
	int getSender();
	int getDestination();
	double getTimeStamp();
	double getLength();
	std::vector<double> getTxTime();
	std::vector<double> getFc();

	
	void UpdateFc(std::vector<double> &, std::vector<double> &);
	void UpdateTxTime(std::vector<double> &);
	double getDratio();
	double getSatisfaction();
};

#line 4 "../Classes/flow.cc"



Flow::Flow(){

}

Flow::~Flow(){

}

void Flow::setType(std::string type){

	_type = type;
}

void Flow::setSender(int sender){

	_sender = sender;
}

void Flow::setDestination(int destination){

	_destination = destination;
}

void Flow::setTimeStamp(double timestamp){

	_timestamp = timestamp;
}

void Flow::setFc(double fc){

	_Fc.push_back(fc);
}

void Flow::setTxTime(double t){

	_TxTime.push_back(t);
}

void Flow::setLength(double dlength){

	_length = dlength;
}

void Flow::setSat(std::vector<double> &s){

	if ((int)_Sat.size() == 0){
		for (int i=0; i<(int)s.size(); i++){
			std::vector<double> inter;
			_Sat.push_back(inter);
		}
	}
	for (int i=0; i<(int)s.size(); i++){
		_Sat[i].push_back(s.at(i));
	}
}

std::string Flow::getType(){

	return _type;
}

int Flow::getSender(){

	return _sender;
}

int Flow::getDestination(){

	return _destination;
}

std::vector<double> Flow::getTxTime(){

	return _TxTime;
}

std::vector<double> Flow::getFc(){

	return _Fc;
}

double Flow::getTimeStamp(){

	return _timestamp;
}

double Flow::getLength(){

	return _length;
}



void Flow::UpdateFc(std::vector<double> &old_fc, std::vector<double> &new_fc){
	
	for (int i=0; i<(int)_Fc.size(); i++){
		for (int j=0; j<(int)old_fc.size(); j++){
			if (old_fc.at(j) == _Fc.at(i)){
				_Fc.at(i) = new_fc.at(j);
				break;
			}
		}
	}
}



void Flow::UpdateTxTime(std::vector<double> &new_times){
	_TxTime.clear();
	for (int i=0; i<(int)new_times.size(); i++){
		_TxTime.push_back(new_times.at(i));
	}
}



double Flow::getDratio(){

	int num_interfaces = 0, counter = 0;
	double avg_flow = 0, sat_interface = 0, dropRatio = 0;
	for (int i=0; i<(int)_Sat.size(); i++){
		for (int j=0; j<(int)_Sat[i].size(); j++){
			if (_Sat[i].at(j) != -1){
				sat_interface += _Sat[i].at(j);   
				counter++;											  
			}
		}

		if (sat_interface != 0){
			avg_flow += sat_interface/counter;						
			num_interfaces++;															
		}
		sat_interface = 0, counter=0;                               
	}

	dropRatio = 1 - ((_length*(avg_flow/num_interfaces))/_length);				
	return dropRatio;
}


double Flow::getSatisfaction(){

  int num_interfaces = 0, counter = 0;
	double avg_flow = 0, sat_interface = 0, avg_satisfaction = 0;
	for (int i=0; i<(int)_Sat.size(); i++){
		for (int j=0; j<(int)_Sat[i].size(); j++){
			if (_Sat[i].at(j) != -1){
				sat_interface += _Sat[i].at(j);   
				counter++;											  
			}
		}

		if (sat_interface != 0){
			avg_flow += sat_interface/counter;						
			num_interfaces++;															
		}
		sat_interface = 0, counter=0;                               
	}
  avg_satisfaction = avg_flow/num_interfaces;				
	return avg_satisfaction;
}

#line 23 "neko.cc"


#line 1 "../Classes/notification.cc"


#line 1 "../Classes/notification.h"














class Notification{

private:
	std::string _type;												
	std::string _flag;												

	int _sender;															
	int _destination;													

	Capabilities _capabilities;								
	Position _coordinates;										

	std::vector<double> _linkfc;							
	std::vector<double> _linkQ;								

	std::vector<double> _ChOcc;								
	std::vector<double> _Sat;									


public:
	Notification(std::string t, int s, int d):_type(t), _sender(s), _destination(d){};

	
	void setPosition(Position &p);
	void setCapabilities(Capabilities &cap);
	void setFlag(std::string);
	void setFc(double);
	void setFc(std::vector<double> &v);
	void setLinkQuality(std::vector<double> &v);
	void setChOcc(std::vector<double> &v);
	void setSat(std::vector<double> &v);

	
	std::string getType();
	std::string getFlag();
	int getSender();
	int getDestination();
	Position getPosition();
	Capabilities getCapabilities();
	std::vector<double> getFc();
	std::vector<double> getLinkQuality();
	std::vector<double> getSat();
	std::vector<double> getChOcc();
};




class AppCTRL{

private:

	std::string _type;

	int _sender;
	int _destination;

	std::string _tprofile;

public:

	AppCTRL(std::string t, int s, int d): _type(t), _sender(s), _destination(d){}

	
	void setTProfile(std::string);

	
	std::string getType();
	int getSender();
	int getDestination();
	std::string getTProfile();
};

#line 2 "../Classes/notification.cc"



void Notification::setPosition(Position &p){

	_coordinates.x = p.x;
	_coordinates.y = p.y;
	_coordinates.z = p.z;
}

void Notification::setCapabilities(Capabilities &c){

	_capabilities.IEEEProtocol = c.IEEEProtocol;
	_capabilities.Multilink = c.Multilink;
	_capabilities.Mlearning = c.Mlearning;
}

void Notification::setLinkQuality(std::vector<double> &v){

	for (int i=0; i<(int)v.size(); i++){
		_linkQ.push_back(v.at(i));
	}
}

void Notification::setFc(std::vector<double> &v){

	for (int i=0; i<(int)v.size(); i++){
		_linkfc.push_back(v.at(i));
	}
}

void Notification::setFc(double fc){

		_linkfc.push_back(fc);
}

void Notification::setFlag(std::string flag){

	_flag = flag;
}
void Notification::setSat(std::vector<double> &sat){

	for (int i=0; i<(int)sat.size(); i++){
		_Sat.push_back(sat.at(i));
	}
}

void Notification::setChOcc(std::vector<double> &occ){

	for (int i=0; i<(int)occ.size(); i++){
		_ChOcc.push_back(occ.at(i));
	}
}


std::string Notification::getType(){

	return _type;
}

int Notification::getSender(){

	return _sender;
}

int Notification::getDestination(){

	return _destination;
}

Position Notification::getPosition(){

	return _coordinates;
}

Capabilities Notification::getCapabilities(){

	return _capabilities;
}

std::vector<double> Notification::getLinkQuality(){

	return _linkQ;
}

std::vector<double> Notification::getFc(){

	return _linkfc;
}
std::string Notification::getFlag(){

	return _flag;
}

std::vector<double> Notification::getSat(){

	return _Sat;
}

std::vector<double> Notification::getChOcc(){

	return _ChOcc;
}




void AppCTRL::setTProfile(std::string p){

	_tprofile = p;
}

std::string AppCTRL::getType(){

	return _type;
}

int AppCTRL::getSender(){

	return _sender;
}

int AppCTRL::getDestination(){

	return _destination;
}

std::string AppCTRL::getTProfile(){

	return _tprofile;
}

#line 24 "neko.cc"


#line 1 "../Classes/interface.h"

class Interface{

public:

  int id;											
	int ChN;										
	int ChW;										
	double fc;										
};


class APInterface : public Interface{

public:
  double TOcc;
  double ChOccSFlows;								
  double ChOccNeighAPs;							

public:
  APInterface();
  ~APInterface();
};

APInterface::APInterface(){
  TOcc = 0;
  ChOccSFlows = 0;
  ChOccNeighAPs = 0;
}

APInterface::~APInterface(){
  
}


class STAInterface : public Interface{

public:
  bool active;									
};

#line 25 "neko.cc"


#line 1 "../Classes/agent.cc"


#line 1 "../Classes/agent.h"

class Agent{

public:
  Agent(std::string type, std::string trgt, std::string func):_type(type), _target(trgt), _OptFunct(func){};

  std::string _type;                                                    
  std::string _target;                                                  
  std::string _OptFunct;                                                

  std::vector<std::vector<double>> ActionSpace;                         
  std::vector<std::vector<double>> Rewards;                             
  std::vector<std::vector<double>> EstRewardAct;                        
  std::vector<std::vector<double>> TimesActSel;                         

  
  void setActSpace();
  

  
  std::string getTarget();
  std::vector<int> getNewAction(std::vector<std::pair<int,double>> &v);   
};

#line 2 "../Classes/agent.cc"



void Agent::setActSpace(){

  if (_type.compare("AP") == 0){
    ActionSpace = Channels;
    for (int i=0; i<(int)ActionSpace.size(); i++){
      std::vector<double> v((int)ActionSpace.at(i).size(), 0.0);
      Rewards.push_back(v);
      TimesActSel.push_back(v);
      EstRewardAct.push_back(v);
    }
  }
}






std::string Agent::getTarget(){
  return _target;
}


std::vector<int> Agent::getNewAction(std::vector<std::pair<int,double>> &ActRew_vector){

  std::vector<int> new_actions;
  for (int i=0; i<(int)ActRew_vector.size(); i++){
    std::pair<int,double> p = ActRew_vector.at(i);
    for (int j=0; j<(int)ActionSpace.size(); j++){
      for (int n=0; n<(int)ActionSpace.at(i).size(); n++){
        if (ActionSpace[j].at(n) == p.first){
          TimesActSel[j].at(n) += 1;
          Rewards[j].at(n) = p.second;
          EstRewardAct[j].at(n) = ((EstRewardAct[j].at(n)*TimesActSel[j].at(n))+Rewards[j].at(n))/(TimesActSel[j].at(n)+2);
          int action_arm = ThompsonSampling(ActionSpace.at(j).size(), &EstRewardAct.at(j), &TimesActSel.at(j));
          new_actions.push_back(ActionSpace[j].at(action_arm));
          
        }
      }
    }
  }

  return new_actions;
}

#line 26 "neko.cc"


#line 1 "../Classes/policy_manager.cc"


#line 1 "../Classes/policy_manager.h"

class PolicyManager {
private:
  std::string _policy;

public:
  PolicyManager ();

  void setType(std::string);
  std::string getType();

  void AllocationFromPolicyEqual(Flow *, std::vector<WifiSTA> &, std::vector<APInterface> &);
  void AllocationFromPolicyOne(Flow *, std::vector<WifiSTA> &, std::vector<APInterface> &);
  void AllocationFromPolicyFixed(Flow *, std::vector<WifiSTA> &, std::vector<APInterface> &);
  void AllocationFromPolicySplit(Flow *, std::vector<WifiSTA> &, std::vector<APInterface> &);

  std::pair<bool,std::vector<double>> AllocationFromPolicyAdaptive(Flow *, std::vector<Flow> &, std::vector<WifiSTA> &, std::vector<APInterface> &, double);
};

#line 2 "../Classes/policy_manager.cc"


PolicyManager::PolicyManager(){

}

void PolicyManager::setType(std::string type){
  _policy = type;
}

std::string PolicyManager::getType(){
  return _policy;
}

void PolicyManager::AllocationFromPolicyEqual(Flow *flow, std::vector<WifiSTA> &AssociatedSTAs, std::vector<APInterface> &InterfaceContainer){
  for (int i=0; i<(int)AssociatedSTAs.size(); i++){
    if (flow->getDestination() == AssociatedSTAs.at(i).id){
      int sta_interfaces = (int)AssociatedSTAs.at(i).fc.size();
      double fLength = flow->getLength();
      double LChunk = fLength/sta_interfaces;
      for (int j=0; j<(int)AssociatedSTAs.at(i).TxRate.size(); j++){
        flow->setTxTime(CalculateTxTime(LChunk, AssociatedSTAs.at(i).TxRate.at(j)));
        flow->setFc(AssociatedSTAs.at(i).fc.at(j));
      }
      break;
    }
  }
}

void PolicyManager::AllocationFromPolicyOne(Flow *flow, std::vector<WifiSTA> &AssociatedSTAs, std::vector<APInterface> &InterfaceContainer){

  double occ_max = 100;
  int interface_index = -1;
  for (int i=0; i<(int)AssociatedSTAs.size(); i++){
    if (flow->getDestination() == AssociatedSTAs.at(i).id){
      if ((int)AssociatedSTAs.at(i).fc.size() > 1){
        for (int j=0; j<(int)AssociatedSTAs.at(i).fc.size(); j++){
          for (int n=0; n<(int)InterfaceContainer.size(); n++){
            if (AssociatedSTAs.at(i).fc.at(j) == InterfaceContainer.at(n).fc){
              if (InterfaceContainer.at(n).TOcc < occ_max) {
                occ_max = InterfaceContainer.at(n).TOcc;
                interface_index = j;
              }
            }
          }
        }
        double fLength = flow->getLength();
        flow->setTxTime(CalculateTxTime(fLength, AssociatedSTAs.at(i).TxRate.at(interface_index)));
        flow->setFc(AssociatedSTAs.at(i).fc.at(interface_index));
      }
      else{
        for (int n=0; n<(int)InterfaceContainer.size(); n++){
          if (AssociatedSTAs.at(i).fc.at(0) == InterfaceContainer.at(n).fc){
            interface_index = InterfaceContainer.at(n).id;
          }
        }
        double fLength = flow->getLength();
        flow->setTxTime(CalculateTxTime(fLength, AssociatedSTAs.at(i).TxRate.at(0)));
        flow->setFc(AssociatedSTAs.at(i).fc.at(0));
      }
      break;
    }
  }
}

void PolicyManager::AllocationFromPolicyFixed(Flow *flow, std::vector<WifiSTA> &AssociatedSTAs, std::vector<APInterface> &InterfaceContainer){

  int sta_index;
  for (int i=0; i<(int)InterfaceContainer.size(); i++){
    if (flow != nullptr){
      for (int j=0; j<(int)AssociatedSTAs.size(); j++){
        if (AssociatedSTAs.at(j).id == flow->getDestination()){
          sta_index = j;
          for (int n=0; n<(int)AssociatedSTAs.at(j).fc.size(); n++){
            if (AssociatedSTAs.at(j).fc.at(n) == InterfaceContainer.at(i).fc){
              flow->setFc(AssociatedSTAs.at(j).fc.at(n));
            }
          }
          break;
        }
      }
    }
  }

  double fLength = flow->getLength();
  std::vector<double> FlowFc = flow->getFc();
  std::vector<double> allocated_load((int)FlowFc.size(), 0.0);
  std::vector<double> interface_id, empty_load;

  while (fLength > 1E-3){
    empty_load.assign((int)FlowFc.size(), 0.0);
    interface_id.clear();
    double length = fLength;
    if (FlowFc.size() > 1){
      for (int j=0; j<(int)FlowFc.size(); j++){
        for (int n=0; n<(int)InterfaceContainer.size(); n++){
          if (FlowFc.at(j) == InterfaceContainer.at(n).fc){
            if (InterfaceContainer.at(n).TOcc + allocated_load.at(j) < 1){
              interface_id.push_back(InterfaceContainer.at(n).id);
              empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
              break;
            }
          }
        }
      }

      if (interface_id.size() == 0){
        for (int j=0; j<(int)FlowFc.size(); j++){
          for (int n=0; n<(int)InterfaceContainer.size(); n++){
            if (FlowFc.at(j) == InterfaceContainer.at(n).fc){
              for (int m=0; m<(int)FlowFc.size(); m++){
                for (int l=0; l<(int)InterfaceContainer.size(); l++){
                  if (FlowFc.at(m) == InterfaceContainer.at(l).fc){
                    if ((std::floor(InterfaceContainer.at(l).TOcc + allocated_load.at(m)) == std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j))) && (InterfaceContainer.at(l).id != InterfaceContainer.at(n).id)){
                      if (interface_id.size() != 0){
                        bool found = false;
                        for (int f=0; f<(int)interface_id.size(); f++){
                          if (interface_id.at(f) == InterfaceContainer.at(n).id){
                            found = true;
                            break;
                          }
                        }
                        if (!found){
                          interface_id.push_back(InterfaceContainer.at(n).id);
                          empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                        }
                      }
                      else{
                        interface_id.push_back(InterfaceContainer.at(n).id);
                        empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                      }
                    }
                    break;
                  }
                }
              }
              break;
            }
          }
        }
      }
      if (interface_id.size() != 0){
        double empty_total = std::accumulate(empty_load.begin(), empty_load.end(), 0.0);
        for (int j=0; j<(int)FlowFc.size(); j++){
          for (int n=0; n<(int)InterfaceContainer.size(); n++){
            if ((FlowFc.at(j) == InterfaceContainer.at(n).fc)){
              double prop = empty_load.at(j)/empty_total;
              for (int m=0; m<(int)AssociatedSTAs.at(sta_index).fc.size(); m++){
                if (AssociatedSTAs.at(sta_index).fc.at(m) == FlowFc.at(j)){
                  double tmp_load = CalculateTxTime(length, AssociatedSTAs.at(sta_index).TxRate.at(m))*prop;
                  if ((std::ceil(InterfaceContainer.at(n).TOcc) == 0) || (InterfaceContainer.at(n).TOcc + allocated_load.at(j) == std::ceil(InterfaceContainer.at(n).TOcc+ allocated_load.at(j)))){
                    allocated_load.at(j) += tmp_load;
                    fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(m));
                  }
                  else if ((InterfaceContainer.at(n).TOcc + allocated_load.at(j) + tmp_load) < std::ceil(InterfaceContainer.at(n).TOcc + allocated_load.at(j))){
                    allocated_load.at(j) += tmp_load;
                    fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(m));
                  }
                  else{
                    double time_left = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                    if (time_left < tmp_load){
                      allocated_load.at(j) += time_left;
                      fLength -= GetFromTxTime(time_left, AssociatedSTAs.at(sta_index).TxRate.at(m));
                    }
                    else{
                      allocated_load.at(j) += tmp_load;
                      fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(m));
                    }
                  }
                  break;
                }
              }
              break;
            }
          }
        }
      }
      else{
        double load = 100;
        int interface = -1;
        for (int j=0; j<(int)FlowFc.size(); j++){
          for (int n=0; n<(int)InterfaceContainer.size(); n++){
            if ((FlowFc.at(j) == InterfaceContainer.at(n).fc) && ((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) < load)){
              load = InterfaceContainer.at(n).TOcc + allocated_load.at(j);
              interface = n;
              break;
            }
          }
        }
        for (int j=0; j<(int)FlowFc.size(); j++){
          if (InterfaceContainer.at(interface).fc == FlowFc.at(j)){
            for (int n=0; n<(int)AssociatedSTAs.at(sta_index).fc.size(); n++){
              if (AssociatedSTAs.at(sta_index).fc.at(n) == FlowFc.at(j)){
                double tmp_load = CalculateTxTime(length, AssociatedSTAs.at(sta_index).TxRate.at(n));
                if ((std::ceil(InterfaceContainer.at(interface).TOcc + allocated_load.at(j)) == 0) || (InterfaceContainer.at(interface).TOcc + allocated_load.at(j) == std::ceil(InterfaceContainer.at(interface).TOcc+ allocated_load.at(j)))){
                  allocated_load.at(j) += tmp_load;
                  fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(n));
                }
                else if ((InterfaceContainer.at(interface).TOcc + allocated_load.at(j) + tmp_load) < std::ceil(InterfaceContainer.at(interface).TOcc + allocated_load.at(j))){
                  allocated_load.at(j) += tmp_load;
                  fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(n));
                }
                else{
                  double time_left = 1-((InterfaceContainer.at(interface).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(interface).TOcc + allocated_load.at(j)));
                  if (time_left < tmp_load){
                    allocated_load.at(j) += time_left;
                    fLength -= GetFromTxTime(time_left, AssociatedSTAs.at(sta_index).TxRate.at(n));
                  }
                  else{
                    allocated_load.at(j) += tmp_load;
                    fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(n));
                  }
                }
                break;
              }
            }
            break;
          }
        }
      }
    }
    else{
      for (int j=0; j<(int)FlowFc.size(); j++){
        for (int n=0; n<(int)AssociatedSTAs.at(sta_index).fc.size(); n++){
          if (AssociatedSTAs.at(sta_index).fc.at(n) == FlowFc.at(j)){
            allocated_load.at(j) = CalculateTxTime(length, AssociatedSTAs.at(sta_index).TxRate.at(n));
            fLength = 0;
          }
        }
      }
    }
  }

  for (int i=0; i<(int)allocated_load.size(); i++){
    flow->setTxTime(allocated_load.at(i));
  }
}

std::pair<bool,std::vector<double>> PolicyManager::AllocationFromPolicyAdaptive(Flow *flow, std::vector<Flow> &OnGoingFlows, std::vector<WifiSTA> &AssociatedSTAs, std::vector<APInterface> &InterfaceContainer, double t){

  std::vector<Flow> ActiveFlows = OnGoingFlows;
  std::vector<double> InterfaceOcc, InterfaceFc;

  for (int i=0; i<(int)InterfaceContainer.size(); i++){
    InterfaceOcc.push_back(InterfaceContainer.at(i).ChOccSFlows);
    InterfaceFc.push_back(InterfaceContainer.at(i).fc);
    InterfaceContainer.at(i).TOcc -= InterfaceContainer.at(i).ChOccSFlows;
    if (InterfaceContainer.at(i).TOcc < 1E-10)
      InterfaceContainer.at(i).TOcc = 0;
    InterfaceContainer.at(i).ChOccSFlows = 0;
    if (flow != nullptr){
      for (int j=0; j<(int)AssociatedSTAs.size(); j++){
        if (AssociatedSTAs.at(j).id == flow->getDestination()){
          for (int n=0; n<(int)AssociatedSTAs.at(j).fc.size(); n++){
            if (AssociatedSTAs.at(j).fc.at(n) == InterfaceContainer.at(i).fc){
              flow->setFc(AssociatedSTAs.at(j).fc.at(n));
            }
          }
          break;
        }
      }
    }
  }

  if (flow != nullptr){
    ActiveFlows.push_back(*flow);
  }

  std::sort(ActiveFlows.begin(), ActiveFlows.end(), [](Flow& lhs, Flow &rhs){
    return lhs.getFc().size() < rhs.getFc().size();
  });

  for (int i=0; i<(int)ActiveFlows.size();i++){

    double fLength = ActiveFlows.at(i).getLength();
    std::vector<double> FlowFc = ActiveFlows.at(i).getFc();
    std::vector<double> allocated_load((int)FlowFc.size(), 0.0);
    std::vector<double> interface_id, empty_load;

    double sta_index;
    for (int j=0; j<(int)AssociatedSTAs.size(); j++){
      if (AssociatedSTAs.at(j).id == ActiveFlows.at(i).getDestination()){
        sta_index = j;
        break;
      }
    }

    while (fLength > 1E-3){
      empty_load.assign((int)FlowFc.size(), 0.0);
      interface_id.clear();
      double length = fLength;
      if (FlowFc.size() > 1){
        for (int j=0; j<(int)FlowFc.size(); j++){
          for (int n=0; n<(int)InterfaceContainer.size(); n++){
            if (FlowFc.at(j) == InterfaceContainer.at(n).fc){
              if (InterfaceContainer.at(n).TOcc + allocated_load.at(j) < 1){
                interface_id.push_back(InterfaceContainer.at(n).id);
                empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
              }
              break;
            }
          }
        }
        if (interface_id.size() == 0){
          for (int j=0; j<(int)FlowFc.size(); j++){
            for (int n=0; n<(int)InterfaceContainer.size(); n++){
              if (FlowFc.at(j) == InterfaceContainer.at(n).fc){
                for (int m=0; m<(int)FlowFc.size(); m++){
                  for (int l=0; l<(int)InterfaceContainer.size(); l++){
                    if (FlowFc.at(m) == InterfaceContainer.at(l).fc){
                      if (InterfaceContainer.at(n).TOcc + allocated_load.at(j) < 1){
                        interface_id.push_back(InterfaceContainer.at(n).id);
                        empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                      }

                      else if ((std::floor(InterfaceContainer.at(l).TOcc + allocated_load.at(m)) == std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j))) && (InterfaceContainer.at(l).id != InterfaceContainer.at(n).id)){
                        if (interface_id.size() != 0){
                          bool found = false;
                          for (int f=0; f<(int)interface_id.size(); f++){
                            if (interface_id.at(f) == InterfaceContainer.at(n).id){
                              found = true;
                              break;
                            }
                          }
                          if (!found){
                            interface_id.push_back(InterfaceContainer.at(n).id);
                            empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                          }
                        }
                        else{
                          interface_id.push_back(InterfaceContainer.at(n).id);
                          empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                        }
                      }
                      break;
                    }
                  }
                }
                break;
              }
            }
          }
        }
        if (interface_id.size() != 0){
          double empty_total = std::accumulate(empty_load.begin(), empty_load.end(), 0.0);
          for (int j=0; j<(int)FlowFc.size(); j++){
            for (int n=0; n<(int)InterfaceContainer.size(); n++){
              if ((FlowFc.at(j) == InterfaceContainer.at(n).fc)){
                double prop = empty_load.at(j)/empty_total;
                for (int m=0; m<(int)AssociatedSTAs.at(sta_index).fc.size(); m++){
                  if (AssociatedSTAs.at(sta_index).fc.at(m) == FlowFc.at(j)){
                    double tmp_load = CalculateTxTime(length, AssociatedSTAs.at(sta_index).TxRate.at(m))*prop;
                    if ((std::ceil(InterfaceContainer.at(n).TOcc) == 0) || (InterfaceContainer.at(n).TOcc + allocated_load.at(j) == std::ceil(InterfaceContainer.at(n).TOcc+ allocated_load.at(j)))){
                      allocated_load.at(j) += tmp_load;
                      fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(m));
                    }
                    else if ((InterfaceContainer.at(n).TOcc + allocated_load.at(j) + tmp_load) < std::ceil(InterfaceContainer.at(n).TOcc + allocated_load.at(j))){
                      allocated_load.at(j) += tmp_load;
                      fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(m));
                    }
                    else{
                      double time_left = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                      if (time_left < tmp_load){
                        allocated_load.at(j) += time_left;
                        fLength -= GetFromTxTime(time_left, AssociatedSTAs.at(sta_index).TxRate.at(m));
                      }
                      else{
                        allocated_load.at(j) += tmp_load;
                        fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(m));
                      }
                    }
                    break;
                  }
                }
                break;
              }
            }
          }
        }
        else{
          double load = 100;
          int interface = -1;
          for (int j=0; j<(int)FlowFc.size(); j++){
            for (int n=0; n<(int)InterfaceContainer.size(); n++){
              if ((FlowFc.at(j) == InterfaceContainer.at(n).fc) && ((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) < load)){
                load = InterfaceContainer.at(n).TOcc + allocated_load.at(j);
                interface = n;
                break;
              }
            }
          }
          for (int j=0; j<(int)FlowFc.size(); j++){
            if (InterfaceContainer.at(interface).fc == FlowFc.at(j)){
              for (int n=0; n<(int)AssociatedSTAs.at(sta_index).fc.size(); n++){
                if (AssociatedSTAs.at(sta_index).fc.at(n) == FlowFc.at(j)){
                  double tmp_load = CalculateTxTime(length, AssociatedSTAs.at(sta_index).TxRate.at(n));
                  if ((std::ceil(InterfaceContainer.at(interface).TOcc + allocated_load.at(j)) == 0) || (InterfaceContainer.at(interface).TOcc + allocated_load.at(j) == std::ceil(InterfaceContainer.at(interface).TOcc+ allocated_load.at(j)))){
                     allocated_load.at(j) += tmp_load;
                     fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(n));
                  }
                  else if ((InterfaceContainer.at(interface).TOcc + allocated_load.at(j) + tmp_load) < std::ceil(InterfaceContainer.at(interface).TOcc + allocated_load.at(j))){
                     allocated_load.at(j) += tmp_load;
                     fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(n));
                  }
                  else{
                    double time_left = 1-((InterfaceContainer.at(interface).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(interface).TOcc + allocated_load.at(j)));
                    if (time_left < tmp_load){
                      allocated_load.at(j) += time_left;
                      fLength -= GetFromTxTime(time_left, AssociatedSTAs.at(sta_index).TxRate.at(n));
                    }
                    else{
                      allocated_load.at(j) += tmp_load;
                      fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(n));
                    }
                  }
                  break;
                }
              }
              break;
            }
          }
        }
      }
      else{
        for (int j=0; j<(int)FlowFc.size(); j++){
          for (int n=0; n<(int)AssociatedSTAs.at(sta_index).fc.size(); n++){
            if (AssociatedSTAs.at(sta_index).fc.at(n) == FlowFc.at(j)){
              allocated_load.at(j) = CalculateTxTime(length, AssociatedSTAs.at(sta_index).TxRate.at(n));
              fLength = 0;
            }
          }
        }
      }
    }

    if (flow != nullptr){
      if (ActiveFlows.at(i).getDestination() == flow->getDestination()){
        for (int j=0; j<(int)allocated_load.size(); j++){
          flow->setTxTime(allocated_load.at(j));
        }
      }
      else{
        for (int j=0; j<(int)FlowFc.size(); j++){
          for (int n=0; n<(int)InterfaceContainer.size(); n++){
            if (FlowFc.at(j) == InterfaceContainer.at(n).fc){
              InterfaceContainer.at(n).ChOccSFlows += allocated_load.at(j);
              InterfaceContainer.at(n).TOcc += allocated_load.at(j);
            }
          }
        }

        for (int j=0; j<(int)OnGoingFlows.size(); j++){
          if (OnGoingFlows.at(j).getDestination() == ActiveFlows.at(i).getDestination()){
            OnGoingFlows.at(j).UpdateTxTime(allocated_load);
            break;
          }
        }
      }
    }
    else{
      for (int j=0; j<(int)FlowFc.size(); j++){
        for (int n=0; n<(int)InterfaceContainer.size(); n++){
          if (FlowFc.at(j) == InterfaceContainer.at(n).fc){
            InterfaceContainer.at(n).ChOccSFlows += allocated_load.at(j);
            InterfaceContainer.at(n).TOcc += allocated_load.at(j);
          }
        }
      }
      for (int j=0; j<(int)OnGoingFlows.size(); j++){
        if (OnGoingFlows.at(j).getDestination() == ActiveFlows.at(i).getDestination()){
          OnGoingFlows.at(j).UpdateTxTime(allocated_load);
          break;
        }
      }
    }
  }
  bool change = false;
  for (int j=0; j<(int)InterfaceOcc.size(); j++){
    InterfaceOcc.at(j) = InterfaceContainer.at(j).ChOccSFlows-InterfaceOcc.at(j);
    if ((InterfaceOcc.at(j) != 0) && (change == false))
      change = true;
  }
  return std::make_pair(change, InterfaceOcc);
}

void PolicyManager::AllocationFromPolicySplit(Flow *flow, std::vector<WifiSTA> &AssociatedSTAs, std::vector<APInterface> &InterfaceContainer){

  std::string type = flow->getType();
  if (type.compare("STREAMING") == 0){
    for (int i=0; i<(int)AssociatedSTAs.size(); i++){
      if (flow->getDestination() == AssociatedSTAs.at(i).id){
        for (int j=0; j<(int)AssociatedSTAs.at(i).fc.size(); j++){
          std::string band = GetBand(AssociatedSTAs.at(i).fc.at(j));
          if (band.compare("6GHz") == 0){
            double fLength = flow->getLength();
            flow->setTxTime(CalculateTxTime(fLength, AssociatedSTAs.at(i).TxRate.at(j)));
            flow->setFc(AssociatedSTAs.at(i).fc.at(j));
            break; 
          }
        }
      }
    }
  }
  else{
    double occ_max = 100;
    int interface_index = -1;
    for (int i=0; i<(int)AssociatedSTAs.size(); i++){
      if (flow->getDestination() == AssociatedSTAs.at(i).id){
        for (int j=0; j<(int)AssociatedSTAs.at(i).fc.size(); j++){
          std::string band = GetBand(AssociatedSTAs.at(i).fc.at(j));
          if (band.compare("6GHz") != 0){
            for (int n=0; n<(int)InterfaceContainer.size(); n++){
              if (AssociatedSTAs.at(i).fc.at(j) == InterfaceContainer.at(n).fc){
                if (InterfaceContainer.at(n).TOcc < occ_max) {
                  occ_max = InterfaceContainer.at(n).TOcc;
                  interface_index = j;
                }
                break;
              }
            }
          }
        }
        double fLength = flow->getLength();
        flow->setTxTime(CalculateTxTime(fLength, AssociatedSTAs.at(i).TxRate.at(interface_index)));
        flow->setFc(AssociatedSTAs.at(i).fc.at(interface_index));
      }
    }
  }
}

#line 27 "neko.cc"


#line 1 "application.h"

#ifndef _APP_
#define _APP_

#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <vector>
#include <string>


#line 122 "application.h"
#endif

#line 28 "neko.cc"


#line 1 "ap.h"

#ifndef _AP_
#define _AP_

std::default_random_engine gen(rand());


#line 611 "ap.h"
#endif

#line 29 "neko.cc"


#line 1 "station.h"
#ifndef _STA_
#define _STA_


#line 330 "station.h"
#endif

#line 30 "neko.cc"


#include "compcxx_neko.h"

#line 7 "ap.h"
class compcxx_AP_5 : public compcxx_component, public TypeII {

public:

	int apID;                                       

	Position coordinates;														
	Capabilities capabilities;											
	Configuration configuration;										
	PolicyManager policy_manager;										

	std::vector<APInterface> InterfaceContainer;		
	std::vector<WifiAP> NeighboringAPs;							
	std::vector<WifiSTA> AssociatedSTAs;						
	std::vector<Flow> OnGoingFlows;									
	

	APStatistics statistics;												

public:

	compcxx_AP_5();																							
	~compcxx_AP_5();																						

	void Setup();																			
	void Start();																			
	void Stop();																			

	
	
	/*inport */void inCtrlApp(AppCTRL &n);								
	/*inport */void inCtrlSTA(Notification &n);						
	/*inport */void inCtrlAP(Notification &n);						
	

	
	class my_AP_outDataSTA_f_t:public compcxx_functor<AP_outDataSTA_f_t>{ public:void  operator() (Flow &q) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(q); return (c[0]->*f[0])(q);};};my_AP_outDataSTA_f_t outDataSTA_f;/*outport void outDataSTA(Flow &q)*/;									
	class my_AP_outCtrlApp_f_t:public compcxx_functor<AP_outCtrlApp_f_t>{ public:void  operator() (AppCTRL &n) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(n); return (c[0]->*f[0])(n);};};my_AP_outCtrlApp_f_t outCtrlApp_f;/*outport void outCtrlApp(AppCTRL &n)*/;							
	class my_AP_outCtrlSTA_f_t:public compcxx_functor<AP_outCtrlSTA_f_t>{ public:void  operator() (Notification &n) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(n); return (c[0]->*f[0])(n);};};my_AP_outCtrlSTA_f_t outCtrlSTA_f;/*outport void outCtrlSTA(Notification &n)*/;					
	class my_AP_outCtrlAP_f_t:public compcxx_functor<AP_outCtrlAP_f_t>{ public:void  operator() (Notification &n) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(n); return (c[0]->*f[0])(n);};};my_AP_outCtrlAP_f_t outCtrlAP_f;/*outport void outCtrlAP(Notification &n)*/;					
	

	
	
	

	
	void Initialization();														
	Flow CreateFlow(int, int, std::string);						
	void AcceptIncomingFlow(Flow &f);									
	void RegisterAT(Flow &f);													
	void NotifyApp(std::string, int);									
	void NotifyNeighbors(std::string, std::vector<double> *o, std::vector<double> *f); 
	void NotifySTA(std::string, Notification *n);			
	
	void Send(Flow &f);																
	void CollectStatistics(Flow *f);									
};

class compcxx_Application_7;/*template <class T> */
#line 267 "../COST/cost.h"
class compcxx_Timer_3 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_3() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_Application_7* p_compcxx_parent;};

class compcxx_Application_7;/*template <class T> */
#line 267 "../COST/cost.h"
class compcxx_Timer_2 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_2() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_Application_7* p_compcxx_parent;};


#line 14 "application.h"
class compcxx_Application_7 : public compcxx_component, public TypeII {

public:

	int srcID;																									
	int destID;																									
	std::string TProfile;																			

public:

	
	compcxx_Application_7();
	~compcxx_Application_7();

	
	void Setup();
	void Start();
	void Stop();

	
	/*inport */void inCtrlAP (AppCTRL &n);

	
	class my_Application_outCtrlAP_f_t:public compcxx_functor<Application_outCtrlAP_f_t>{ public:void  operator() (AppCTRL &n) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(n); return (c[0]->*f[0])(n);};};my_Application_outCtrlAP_f_t outCtrlAP_f;/*outport void outCtrlAP (AppCTRL &n)*/;

	
	compcxx_Timer_2 /*<trigger_t> */start;
	compcxx_Timer_3 /*<trigger_t> */end;

	/*inport */inline void Start(trigger_t&);
	/*inport */inline void End(trigger_t&);
};

class compcxx_Neko_9;/*template <class T> */
#line 267 "../COST/cost.h"
class compcxx_Timer_8 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_8() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_Neko_9* p_compcxx_parent;};

class compcxx_STA_6;/*template <class T> */
#line 267 "../COST/cost.h"
class compcxx_Timer_4 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_4() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_STA_6* p_compcxx_parent;};


#line 4 "station.h"
class compcxx_STA_6 : public compcxx_component, public TypeII {

public:

	int staID;																						
	int servingAP;																				
	std::string state;																		
	std::string traffic_type;															

	Position coordinates;																	
	Configuration configuration;													
	Capabilities capabilities;														

	std::vector<STAInterface> InterfaceContainer;					
	std::vector<WifiAP> InRangeAPs;												
	std::deque <Notification> Queue;

	Flow flow;																						
	STAStatistics statistics;															

public:

	compcxx_STA_6();
	~compcxx_STA_6();

	void Setup();																					
	void Start();																					
	void Stop();																					

	
	/*inport */void inCrtlAP(Notification &n);								
	/*inport */void inDataAP(Flow &f);												

	
	class my_STA_outCtrlAP_f_t:public compcxx_functor<STA_outCtrlAP_f_t>{ public:void  operator() (Notification &n) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(n); return (c[0]->*f[0])(n);};};my_STA_outCtrlAP_f_t outCtrlAP_f;/*outport void outCtrlAP(Notification &n)*/;							

	
	
	

	compcxx_Timer_4 /*<trigger_t> */WaitProbes;
	/*inport */inline void Discovery(trigger_t&);

	
	void Initialization();																
	void NotifyAP(std::string, std::vector<double> *v);
	void DoMLOSetup(Notification &n);
	void UpdateInterfaces(Notification &n);
	void CalculateStats();
};


#line 32 "neko.cc"
class compcxx_Neko_9 : public compcxx_component, public CostSimEng {

public:

	
	void Setup();
	void Start();
	void Stop();

	
	void GenerateRandom();
	void GenerateResultReport();
	

public:
	int seed;

	compcxx_array<compcxx_AP_5  >APContainer;
  compcxx_array<compcxx_STA_6  >STAContainer;
	compcxx_array<compcxx_Application_7  >AppContainer;

	compcxx_Timer_8 /*<trigger_t> */Progress;
	/*inport */inline void ProgressBar(trigger_t&);
};


#line 66 "ap.h"
compcxx_AP_5::compcxx_AP_5 (){
	
}


#line 70 "ap.h"
compcxx_AP_5::~compcxx_AP_5(){

}


#line 74 "ap.h"
void compcxx_AP_5::Setup(){

}


#line 78 "ap.h"
void compcxx_AP_5::Start(){
	Initialization();
}


#line 82 "ap.h"
void compcxx_AP_5::Stop(){

}












#line 96 "ap.h"
void compcxx_AP_5::Initialization(){

	int intSz = maxIntNum;
	for (int i=0; i<intSz; i++){
		
		

		APInterface interface;
		interface.id = i;
		
		interface.ChN = Channels[i].at(0);
		std::pair<double, int> p = GetFromChN(interface.ChN);
		interface.fc = p.first;
		interface.ChW = p.second;
		InterfaceContainer.push_back(interface);
	}

	if (capabilities.Multilink){
		policy_manager.setType(policy);
	}
	else{
		policy_manager.setType("ALL_ONE");
	}

	NotifyNeighbors("AP_NEIGHBOR_DISCOVERY", nullptr, nullptr);
}











#line 132 "ap.h"
void compcxx_AP_5::NotifyNeighbors(std::string type,std::vector<double> *occ, std::vector<double> *fc){

	if (type.compare("AP_NEIGHBOR_DISCOVERY") == 0){
		std::vector<double> int_fc;
		for (int i=0; i<(int)InterfaceContainer.size(); i++){
			int_fc.push_back(InterfaceContainer.at(i).fc);
		}
		Notification notification ("AP_NEIGHBOR_DISCOVERY", apID, -1);
		notification.setPosition(coordinates);
		notification.setCapabilities(capabilities);
		notification.setFc(int_fc);
		(outCtrlAP_f(notification));
	}
	else if ((type.compare("FLOW_START") == 0) || (type.compare("ADD") == 0)){
		for (int i=0; i<(int)NeighboringAPs.size(); i++){
			Notification notification ("AP_LOAD", apID, NeighboringAPs.at(i).id);
			notification.setFlag("ADD");
			notification.setChOcc(*occ);
			notification.setFc(*fc);
			(outCtrlAP_f(notification));
		}
	}
	else if ((type.compare("FLOW_END") == 0) || (type.compare("REMOVE") == 0)){
		for (int i=0; i<(int)NeighboringAPs.size(); i++){
			Notification notification ("AP_LOAD", apID, NeighboringAPs.at(i).id);
			notification.setFlag("REMOVE");
			notification.setChOcc(*occ);
			notification.setFc(*fc);
			(outCtrlAP_f(notification));
		}
	}
}













#line 176 "ap.h"
void compcxx_AP_5::inCtrlAP(Notification &n){

	std::string type = n.getType();

	if ((n.getDestination() == -1)){
		if(type.compare("AP_NEIGHBOR_DISCOVERY") == 0){

			Position sender_coordinates = n.getPosition();
			std::vector<double> sender_Fc = n.getFc();
			std::vector<double> overlapping_fc;

			for (int i=0; i<(int)sender_Fc.size(); i++){
				double RxRSSI = CalculateRSSI(configuration.TxPower, sender_Fc.at(i), coordinates.x, coordinates.y, coordinates.z, sender_coordinates.x, sender_coordinates.y, sender_coordinates.z);
				if (RxRSSI >= configuration.CCA){
					overlapping_fc.push_back(sender_Fc.at(i));
				}
			}
			if ((int)overlapping_fc.size() !=0){
				WifiAP ap;
				ap.id = n.getSender();
				ap.coord = n.getPosition();
				ap.ChOcc.assign(overlapping_fc.size(), 0.0);
				for (int i=0; i<(int)overlapping_fc.size(); i++){
					ap.fc.push_back(overlapping_fc.at(i));
				}
				NeighboringAPs.push_back(ap);
			}
		}
	}
	else{
		if (n.getDestination() == apID){
			if (type.compare("AP_LOAD") == 0){
				bool update_load = false;
				for (int i=0; i<(int)NeighboringAPs.size(); i++){
					if (n.getSender() == NeighboringAPs.at(i).id){
						if (NeighboringAPs.at(i).fc.size()!=0){

							std::vector<double> neigh_occ = n.getChOcc();
							std::vector<double> neigh_fc = n.getFc();
							std::string t_flag = n.getFlag();
							for (int j=0; j<(int)NeighboringAPs.at(i).fc.size(); j++){
								for (int n=0; n<(int)neigh_fc.size(); n++){
									if ((NeighboringAPs.at(i).fc.at(j) == neigh_fc.at(n)) && (t_flag.compare("ADD")==0)){
										update_load = true;
										NeighboringAPs.at(i).ChOcc.at(j) += neigh_occ.at(n);
										break;
									}
									else if ((NeighboringAPs.at(i).fc.at(j) == neigh_fc.at(n)) && (t_flag.compare("REMOVE")==0)){
										update_load = true;
										NeighboringAPs.at(i).ChOcc.at(j) -= neigh_occ.at(n);
										if (NeighboringAPs.at(i).ChOcc.at(j) < 1E-10)
											NeighboringAPs.at(i).ChOcc.at(j) = 0;
										break;
									}
								}
							}
						}
					}
				}

				if (update_load){
					for (int i=0; i<(int)InterfaceContainer.size(); i++){
						InterfaceContainer.at(i).TOcc -= InterfaceContainer.at(i).ChOccNeighAPs;
						InterfaceContainer.at(i).ChOccNeighAPs = 0;
						std::string Int_band = GetBand(InterfaceContainer.at(i).fc);
						for (int j=0; j<(int)NeighboringAPs.size(); j++){
							for (int n=0; n<(int)NeighboringAPs.at(j).fc.size(); n++){
								std::string Neigh_band = GetBand(NeighboringAPs.at(j).fc.at(n));
								if (Int_band.compare(Neigh_band) == 0){
									if (CheckNeighChOverlapp(InterfaceContainer.at(i).fc, NeighboringAPs.at(j).fc.at(n))){
										InterfaceContainer.at(i).ChOccNeighAPs += NeighboringAPs.at(j).ChOcc.at(n);
									}
									break;
								}
							}
						}
						InterfaceContainer.at(i).TOcc += InterfaceContainer.at(i).ChOccNeighAPs;
					}
				}
				NotifySTA("SAT_UPDATE", nullptr);
			}
			else if (type.compare("CHANNEL_SWITCH_AP") == 0){
				for (int i=0; i<(int)NeighboringAPs.size(); i++){
					if (n.getSender() == NeighboringAPs.at(i).id){
						NeighboringAPs.at(i).fc.clear(); 
						std::vector<double> neigh_fc = n.getFc(); 
						for (int j=0; j<(int)neigh_fc.size(); j++){
							double RxRSSI = CalculateRSSI(configuration.TxPower, neigh_fc.at(j), coordinates.x, coordinates.y, coordinates.z, NeighboringAPs.at(i).coord.x, NeighboringAPs.at(i).coord.y, NeighboringAPs.at(i).coord.z);
							if (RxRSSI >= configuration.CCA){
								NeighboringAPs.at(i).fc.push_back(neigh_fc.at(j));
							}
						}
						break;
					}
				}
			}
		}
	}
}







#line 281 "ap.h"
Flow compcxx_AP_5::CreateFlow(int src, int dest, std::string type){

	Flow flow;
	flow.setSender(src);
	flow.setDestination(dest);
	flow.setType(type);
	flow.setTimeStamp(SimTime());

	if (type.compare("STREAMING")==0){
		std::uniform_real_distribution<double>BWGen(1, 1);
		flow.setLength(BWGen(gen));
	}
	else{
		std::uniform_real_distribution<double>BWGen(2, medBW);
		flow.setLength(BWGen(gen));
	}

	return flow;
}








#line 307 "ap.h"
void compcxx_AP_5::AcceptIncomingFlow(Flow &f){

	std::string type = f.getType();
	if ((type.compare("STREAMING") == 0)||(type.compare("ELASTIC") == 0)){
		OnGoingFlows.push_back(f);
		RegisterAT(f);
	}
}









#line 323 "ap.h"
void compcxx_AP_5::RegisterAT(Flow &f){

	std::vector<double> FTxTimes = f.getTxTime();
	std::vector<double> FlowFc = f.getFc();

	for (int i=0; i<(int)FlowFc.size(); i++){
		for (int j=0; j<(int)InterfaceContainer.size(); j++){
			if (FlowFc.at(i) == InterfaceContainer.at(j).fc){
				InterfaceContainer.at(j).ChOccSFlows += FTxTimes.at(i);
				InterfaceContainer.at(j).TOcc = InterfaceContainer.at(j).ChOccSFlows + InterfaceContainer.at(j).ChOccNeighAPs;
				break;
			}
		}
	}
	Send(f);
	NotifySTA("SAT_UPDATE", nullptr);
	NotifyNeighbors("FLOW_START", &FTxTimes, &FlowFc);
}










#line 350 "ap.h"
void compcxx_AP_5::NotifySTA(std::string type, Notification *n){

	if (type.compare("PROBE_RESP") == 0){
		Notification notification ("PROBE_RESP", apID, n->getSender());
		notification.setPosition(coordinates);
		notification.setCapabilities(capabilities);
		notification.setFc(InterfaceContainer.at(0).fc);
		(outCtrlSTA_f(notification));
	}
	else if (type.compare("CONFIG_RESP") == 0){
		Notification notification ("CONFIG_RESP", apID, n->getSender());
		notification.setPosition(coordinates);
		for (int i=0; i<(int)InterfaceContainer.size(); i++){
			notification.setFc(InterfaceContainer.at(i).fc);
		}
		(outCtrlSTA_f(notification));
	}
	else if (type.compare("MLO_SETUP_RESP") == 0){
		for (int i=0; i<(int)AssociatedSTAs.size(); i++){
			if (AssociatedSTAs.at(i).id == n->getSender()){

				
				AssociatedSTAs.at(i).fc.clear();
				AssociatedSTAs.at(i).RSSI.clear();
				AssociatedSTAs.at(i).SNR.clear();
				AssociatedSTAs.at(i).TxRate.clear();

				std::vector<double> sta_linkQ = n->getLinkQuality();
				for (int j=0; j<(int)sta_linkQ.size(); j++){
					if (sta_linkQ.at(j) >= configuration.CCA){
						double Dl_RSSI = CalculateRSSI(configuration.TxPower, InterfaceContainer.at(j).fc, coordinates.x, coordinates.y, coordinates.z, AssociatedSTAs.at(i).coord.x, AssociatedSTAs.at(i).coord.y, AssociatedSTAs.at(i).coord.z);
						double Dl_SNR = CalculateSNR(Dl_RSSI, InterfaceContainer.at(j).ChW);
						double Dl_TxRate = CalculateDataRate(Dl_SNR, InterfaceContainer.at(j).fc, InterfaceContainer.at(j).ChW, capabilities, configuration);

						if (Dl_TxRate != 0){
							AssociatedSTAs.at(i).fc.push_back(InterfaceContainer.at(j).fc);
							AssociatedSTAs.at(i).RSSI.push_back(Dl_RSSI);
							AssociatedSTAs.at(i).SNR.push_back(Dl_SNR);
							AssociatedSTAs.at(i).TxRate.push_back(Dl_TxRate);
						}
					}
				}

				
				NotifyApp("CTRL_START", n->getSender());


				
				Notification notification ("MLO_SETUP_RESP", apID, n->getSender());
				notification.setFc(AssociatedSTAs.at(i).fc);
				(outCtrlSTA_f(notification));
			}
		}
	}
	else if (type.compare("SAT_UPDATE") == 0){

		for (int i=0; i<(int)OnGoingFlows.size(); i++){
			std::vector<double> fc = OnGoingFlows.at(i).getFc();
			std::vector<double> txtimes = OnGoingFlows.at(i).getTxTime();
			std::vector<double> ATSat((int)InterfaceContainer.size(), -1.0);

			for (int j=0; j<(int)fc.size(); j++){
				for (int n=0; n<(int)InterfaceContainer.size(); n++){
					
					if ((fc.at(j) == InterfaceContainer.at(n).fc) && (txtimes.at(j) != 0)){
						double Sat = std::min(1.0,InterfaceContainer.at(n).TOcc)/InterfaceContainer.at(n).TOcc;
						ATSat.at(n) = Sat;
						break;
					}
				}
			}
			OnGoingFlows.at(i).setSat(ATSat);
			Notification notification ("SAT_UPDATE", apID,  OnGoingFlows.at(i).getDestination());
			notification.setSat(ATSat);
			(outCtrlSTA_f(notification));
		}
		CollectStatistics(nullptr);
	}
	else if (type.compare("FLOW_END") == 0){
		(outCtrlSTA_f(*n));
	}
}










#line 441 "ap.h"
void compcxx_AP_5::NotifyApp(std::string type, int destination){

	if (type.compare("CTRL_START") == 0){
		for (int i=0; i<(int)AssociatedSTAs.size(); i++){
			if (destination == AssociatedSTAs.at(i).id){
				AppCTRL app_ctrl("CTRL_START", apID, destination);
				app_ctrl.setTProfile(AssociatedSTAs.at(i).traffic_type);
				(outCtrlApp_f(app_ctrl));
				break;
			}
		}
	}
	else{
		AppCTRL app_ctrl ("CTRL_CANCEL", apID, destination);
		(outCtrlApp_f(app_ctrl));
	}
}










#line 467 "ap.h"
void compcxx_AP_5::inCtrlSTA(Notification &n){

	std::string type = n.getType();

	if (n.getDestination() == apID){
		if (type.compare("PROBE_REQ") == 0){
			NotifySTA("PROBE_RESP", &n);
		}
		if (type.compare("CONFIG_REQ") == 0){
			NotifySTA("CONFIG_RESP", &n);
		}
		else if(type.compare("MLO_SETUP_REQ") == 0){
			NotifySTA("MLO_SETUP_RESP", &n);
		}
		else if(type.compare("UPDATE_MLO") == 0){
			NotifySTA("MLO_SETUP_RESP", &n);
		}
		else if (type.compare("STA_DEASSOCIATION") == 0){
			for (int i=0; i<(int)AssociatedSTAs.size(); i++){
				if (n.getSender() == AssociatedSTAs.at(i).id){
					AssociatedSTAs.erase(AssociatedSTAs.begin()+i);
					NotifyApp("CTRL_CANCEL", n.getSender());
				}
			}
		}
	}
	else if ((n.getDestination() == -1) && (type.compare("PROBE_REQ") == 0)){
		NotifySTA("PROBE_RESP", &n);
	}
}








#line 504 "ap.h"
void compcxx_AP_5::inCtrlApp(AppCTRL &n){

	if (n.getSender() == apID){
		std::string type = n.getType();
		if(type.compare("FLOW_START") == 0){
			for (int i=0; i<(int)AssociatedSTAs.size(); i++){
				if (n.getDestination() == AssociatedSTAs.at(i).id){

					Flow flow = CreateFlow(apID, n.getDestination(), AssociatedSTAs.at(i).traffic_type);
					std::string policy_type = policy_manager.getType();

					if (policy_type.compare("MSLA") == 0){
						policy_manager.AllocationFromPolicyEqual(&flow, AssociatedSTAs, InterfaceContainer);
					}
					else if (policy_type.compare("SLCI") == 0){
						policy_manager.AllocationFromPolicyOne(&flow, AssociatedSTAs, InterfaceContainer);
					}
					else if (policy_type.compare("MCAA") == 0){
						policy_manager.AllocationFromPolicyFixed(&flow, AssociatedSTAs, InterfaceContainer);
					}
					else if (policy_type.compare("VIDEO_DATA_SPLIT") == 0){
						policy_manager.AllocationFromPolicySplit(&flow, AssociatedSTAs, InterfaceContainer);
					}

					AcceptIncomingFlow(flow);
					break;
				}
			}
		}
		else{
			for (int i=0; i<(int)OnGoingFlows.size(); i++){
				if (n.getDestination() == OnGoingFlows.at(i).getDestination()){

					std::vector<double> FlowFc = OnGoingFlows.at(i).getFc();
					std::vector<double> FTxTimes = OnGoingFlows.at(i).getTxTime();

					for (int j=0; j<(int)FlowFc.size(); j++){
						for (int m=0; m<(int)InterfaceContainer.size(); m++){
							if (FlowFc.at(j) == InterfaceContainer.at(m).fc){
								InterfaceContainer.at(m).ChOccSFlows -= FTxTimes.at(j);
								if (InterfaceContainer.at(m).ChOccSFlows < 1E-10){
			            InterfaceContainer.at(m).ChOccSFlows = 0;
			          }
								InterfaceContainer.at(m).TOcc = InterfaceContainer.at(m).ChOccSFlows + InterfaceContainer.at(m).ChOccNeighAPs;
							}
						}
					}

					Notification notification ("FLOW_END", apID, OnGoingFlows.at(i).getDestination());
					NotifySTA("FLOW_END", &notification);
					NotifyNeighbors("FLOW_END", &FTxTimes, &FlowFc);
					CollectStatistics(&OnGoingFlows.at(i));
					OnGoingFlows.erase(OnGoingFlows.begin()+i);
					break;
				}
			}
		}
	}
}






#line 568 "ap.h"
void compcxx_AP_5::Send(Flow &f){
	(outDataSTA_f(f));
}







#line 577 "ap.h"
void compcxx_AP_5::CollectStatistics(Flow *flow){

 if (flow != nullptr){
	 double drop_Ratio = flow->getDratio();
	 statistics.AvgDRPerFlow.push_back(drop_Ratio);
	 std::vector<double> ChOcc((int)InterfaceContainer.size(), -1), ChRew((int)InterfaceContainer.size(), -1);
	 for (int i=0; i<(int)InterfaceContainer.size(); i++){
		 ChOcc.at(i) = std::min(1.0,InterfaceContainer.at(i).TOcc);
		 ChRew.at(i) = std::max(0.0,1-InterfaceContainer.at(i).TOcc);
	 }
	 statistics.ChOcc.push_back(ChOcc);
	 statistics.ChReward.push_back(ChRew);
	 statistics.SimT.push_back(SimTime());
 }
 else{
	 std::vector<double> ChOcc((int)InterfaceContainer.size(), -1), ChRew((int)InterfaceContainer.size(), -1);
	 for (int i=0; i<(int)InterfaceContainer.size(); i++){
		 ChOcc.at(i) = std::min(1.0,InterfaceContainer.at(i).TOcc);
		 ChRew.at(i) = std::max(0.0,1-InterfaceContainer.at(i).TOcc);
	 }
	 statistics.ChOcc.push_back(ChOcc);
	 statistics.ChReward.push_back(ChRew);
	 statistics.SimT.push_back(SimTime());
	}
}










#line 288 "../COST/cost.h"

#line 288 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_3/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 300 "../COST/cost.h"

#line 300 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_3/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 311 "../COST/cost.h"

#line 311 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_3/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 319 "../COST/cost.h"

#line 319 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_3/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->End(m_event.data));
}




#line 288 "../COST/cost.h"

#line 288 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_2/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 300 "../COST/cost.h"

#line 300 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_2/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 311 "../COST/cost.h"

#line 311 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_2/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 319 "../COST/cost.h"

#line 319 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_2/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->Start(m_event.data));
}




#line 43 "application.h"

#line 44 "application.h"

#line 47 "application.h"
compcxx_Application_7::compcxx_Application_7(){

	start.p_compcxx_parent=this /*connect start.to_component,*/;
	end.p_compcxx_parent=this /*connect end.to_component,*/;
}


#line 53 "application.h"
compcxx_Application_7::~compcxx_Application_7(){

}


#line 57 "application.h"
void compcxx_Application_7::Setup(){

	
}


#line 62 "application.h"
void compcxx_Application_7::Start(){

	
}


#line 67 "application.h"
void compcxx_Application_7::Stop(){

	
}


#line 72 "application.h"
void compcxx_Application_7::Start(trigger_t&){

	
	AppCTRL ctrl("FLOW_START", srcID, destID);
	(outCtrlAP_f(ctrl));

	

	if (TProfile.compare("STREAMING") != 0){
		end.Set(SimTime()+Exponential(t_EndFlow));
	}
	else{
		end.Set(runTime-(1E-3));
	}
}



#line 89 "application.h"
void compcxx_Application_7::End(trigger_t&){

	
	AppCTRL ctrl("FLOW_END", srcID, destID);
	(outCtrlAP_f(ctrl));

	
	start.Set(SimTime()+Exponential(t_ActFlow));
}


#line 99 "application.h"
void compcxx_Application_7::inCtrlAP (AppCTRL &m){

	if (m.getDestination() == destID){

		std::string type = m.getType();

		if (type.compare("CTRL_START") == 0){

			srcID = m.getSender();
			TProfile = m.getTProfile();

			if ((TProfile.compare("STREAMING") == 0)||(TProfile.compare("ELASTIC") == 0)){
				start.Set(SimTime()+Exponential(t_ActFlow));
			}
		}
		else if (type.compare("CTRL_CANCEL") == 0){
			
			if (start.Active())
				start.Cancel();
		}
	}
}


#line 288 "../COST/cost.h"

#line 288 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_8/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 300 "../COST/cost.h"

#line 300 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_8/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 311 "../COST/cost.h"

#line 311 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_8/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 319 "../COST/cost.h"

#line 319 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_8/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->ProgressBar(m_event.data));
}




#line 288 "../COST/cost.h"

#line 288 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_4/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 300 "../COST/cost.h"

#line 300 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_4/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 311 "../COST/cost.h"

#line 311 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_4/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 319 "../COST/cost.h"

#line 319 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_4/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->Discovery(m_event.data));
}




#line 45 "station.h"

#line 55 "station.h"
compcxx_STA_6::compcxx_STA_6 (){
	WaitProbes.p_compcxx_parent=this /*connect WaitProbes.to_component,*/;
	
}


#line 60 "station.h"
compcxx_STA_6::~compcxx_STA_6 (){
}


#line 63 "station.h"
void compcxx_STA_6::Setup(){

}


#line 67 "station.h"
void compcxx_STA_6::Start(){
	Initialization();
}


#line 71 "station.h"
void compcxx_STA_6::Stop(){
}








#line 80 "station.h"
void compcxx_STA_6::Initialization(){

	if (capabilities.Multilink){
		int intSz = maxIntNum;
		for (int i=0; i<intSz; i++){
			STAInterface interface;
			interface.id = i;
			InterfaceContainer.push_back(interface);
		}
	}
	else{
		STAInterface interface;
		interface.id = 0;
		interface.active = true;
		InterfaceContainer.push_back(interface);
	}
	NotifyAP("PROBE_REQ", nullptr);
	NotifyAP("CONFIG_REQ", nullptr);
}








#line 106 "station.h"
void compcxx_STA_6::NotifyAP(std::string type, std::vector<double> *v){

	if (type.compare("PROBE_REQ") == 0){
		Notification notification ("PROBE_REQ", staID, -1);
		(outCtrlAP_f(notification));
	}
	else if (type.compare("CONFIG_REQ") == 0){
		Notification notification ("CONFIG_REQ", staID, servingAP);
		(outCtrlAP_f(notification));
	}
	else if (type.compare("MLO_SETUP_REQ") == 0){
		Notification notification ("MLO_SETUP_REQ", staID, servingAP);
		notification.setLinkQuality(*v);
		(outCtrlAP_f(notification));
	}
	else if (type.compare("UPDATE_MLO") == 0){
		Notification notification ("UPDATE_MLO", staID, servingAP);
		notification.setLinkQuality(*v);
		(outCtrlAP_f(notification));
	}
}








#line 134 "station.h"
void compcxx_STA_6::inCrtlAP(Notification &n){

	if (n.getDestination() == staID){
		std::string type = n.getType();
		if (type.compare("PROBE_RESP") == 0){
			Queue.push_back(n);
			if (!WaitProbes.Active()){
				WaitProbes.Set(SimTime());
			}
		}
		else if (type.compare("CONFIG_RESP") == 0){
			if (!capabilities.Multilink){
				std::vector<double> fc = n.getFc();
				InterfaceContainer.at(0).fc = fc.at(0);
			}
			else{
				DoMLOSetup(n);
			}
		}
		else if (type.compare("MLO_SETUP_RESP") == 0){

			for (int i=0; i<(int)InterfaceContainer.size(); i++){
				InterfaceContainer.at(i).active = false;
			}

			std::vector<double> LinkFc = n.getFc();
			for (int i=0; i<(int)LinkFc.size(); i++){
				std::string band = GetBand(LinkFc.at(i));
				if (band.compare("2_4GHz") == 0){
					InterfaceContainer.at(i).fc = LinkFc.at(i);
					InterfaceContainer.at(i).active = true;
				}
				else if (band.compare("5GHz") == 0){
					InterfaceContainer.at(i).fc = LinkFc.at(i);
					InterfaceContainer.at(i).active = true;
				}
				else if (band.compare("6GHz") == 0){
					InterfaceContainer.at(i).fc = LinkFc.at(i);
					InterfaceContainer.at(i).active = true;
				}
			}
		}
		else if(type.compare("CHANNEL_SWITCH_STA") == 0){

			for (int i=0; i<(int)InterfaceContainer.size(); i++){
				InterfaceContainer.at(i).active = false;
			}
			UpdateInterfaces(n);
		}
		else if ((type.compare("SAT_UPDATE") == 0) && (state.compare("ACTIVE") == 0)){

			std::vector<double> satisfaction = n.getSat();
			flow.setSat(satisfaction);

			std::vector<double> Fc = n.getFc();
			std::vector<double> sat_evolution (InterfaceContainer.size(), 0.0);
			std::vector<double> sim_time (InterfaceContainer.size(), 0.0);

			for (int j=0; j<(int)InterfaceContainer.size(); j++){
				if ((satisfaction.at(j) != -1) && (InterfaceContainer.at(j).active)){
					sat_evolution.at(j) = satisfaction.at(j);
					sim_time.at(j) = SimTime();
					break;
				}
			}
			statistics.SatEvo.push_back(sat_evolution);
			statistics.SimT.push_back(sim_time);
		}
		else if(type.compare("FLOW_END") == 0){
			state = "IDLE";
			CalculateStats();
		}
	}
	else{
		std::string type = n.getType();
		if ((type.compare("SAT_UPDATE") == 0) && (state.compare("ACTIVE") == 0)){

			std::vector<double> satisfaction = n.getSat();
			flow.setSat(satisfaction);

			std::vector<double> Fc = n.getFc();
			std::vector<double> sat_evolution (InterfaceContainer.size(), 0.0);
			std::vector<double> sim_time (InterfaceContainer.size(), 0.0);

			for (int j=0; j<(int)InterfaceContainer.size(); j++){
				if ((satisfaction.at(j) != -1) && (InterfaceContainer.at(j).active)){
					sat_evolution.at(j) = satisfaction.at(j);
					sim_time.at(j) = SimTime();
					break;
				}
			}
			statistics.SatEvo.push_back(sat_evolution);
			statistics.SimT.push_back(sim_time);
		}
	}
}








#line 237 "station.h"
void compcxx_STA_6::inDataAP(Flow &f){
	if (f.getDestination() == staID){
		flow = f;
		state = "ACTIVE";
	}
}








#line 250 "station.h"
void compcxx_STA_6::Discovery(trigger_t&){
	while (Queue.size() > 0) {
		Notification n = Queue.front();
		Position AP_coord = n.getPosition();
		std::vector<double> fc = n.getFc();

		double RSSI = CalculateRSSI(configuration.TxPower, fc.at(0), AP_coord.x, AP_coord.y, AP_coord.z, coordinates.x, coordinates.y, coordinates.z);
		if (RSSI >= RSSIth){
			WifiAP ap;
			ap.id = n.getSender();
			ap.coord = AP_coord;
			InRangeAPs.push_back(ap);
		}
		Queue.pop_front();
	}
}








#line 273 "station.h"
void compcxx_STA_6::DoMLOSetup(Notification &n){

	Position AP_coord = n.getPosition();
	std::vector<double> fc = n.getFc();
	std::vector<double> linkQ;

	for (int i=0; i<(int)fc.size(); i++){
		double RSSI = CalculateRSSI(configuration.TxPower, fc.at(i), AP_coord.x, AP_coord.y, AP_coord.z, coordinates.x, coordinates.y, coordinates.z);
		linkQ.push_back(RSSI);
	}

	NotifyAP("MLO_SETUP_REQ", &linkQ);
}








#line 293 "station.h"
void compcxx_STA_6::UpdateInterfaces(Notification &n){

	if (!capabilities.Multilink){
		std::vector<double> fc = n.getFc();
		InterfaceContainer.at(0).active = true;
		InterfaceContainer.at(0).fc = fc.at(0);
	}
	else{
		for (int i=0; i<(int)InRangeAPs.size(); i++){
			if (servingAP == InRangeAPs.at(i).id){
				std::vector<double> fc = n.getFc();
				std::vector<double> linkQ;

				for (int j=0; j<(int)fc.size(); j++){
					double RSSI = CalculateRSSI(configuration.TxPower, fc.at(j), InRangeAPs.at(i).coord.x, InRangeAPs.at(i).coord.y, InRangeAPs.at(i).coord.z, coordinates.x, coordinates.y, coordinates.z);
					linkQ.push_back(RSSI);
				}
				NotifyAP("UPDATE_MLO", &linkQ);
				break;
			}
		}
	}
}








#line 323 "station.h"
void compcxx_STA_6::CalculateStats(){

	double satisfaction = flow.getSatisfaction();
	statistics.AvgSatPerFlow.push_back(satisfaction);
	statistics.AvgThPerFlow.push_back(satisfaction*flow.getLength());
}


#line 54 "neko.cc"

#line 57 "neko.cc"
void compcxx_Neko_9 :: Setup(){

	if (rnd){
		GenerateRandom();
	}

	for (int i=0; i<(int)APContainer.size(); i++){
		for (int j=0; j<(int)STAContainer.size(); j++){
			APContainer[i].outCtrlSTA_f.Connect(STAContainer[j],(compcxx_component::AP_outCtrlSTA_f_t)&compcxx_STA_6::inCrtlAP) /*connect APContainer[i].outCtrlSTA,STAContainer[j].inCrtlAP*/;
			APContainer[i].outDataSTA_f.Connect(STAContainer[j],(compcxx_component::AP_outDataSTA_f_t)&compcxx_STA_6::inDataAP) /*connect APContainer[i].outDataSTA,STAContainer[j].inDataAP*/;
			STAContainer[j].outCtrlAP_f.Connect(APContainer[i],(compcxx_component::STA_outCtrlAP_f_t)&compcxx_AP_5::inCtrlSTA) /*connect STAContainer[j].outCtrlAP,APContainer[i].inCtrlSTA*/;
		}

		for (int j=0; j<(int)APContainer.size(); j++){
			if (APContainer[i].apID != APContainer[j].apID){
				APContainer[i].outCtrlAP_f.Connect(APContainer[j],(compcxx_component::AP_outCtrlAP_f_t)&compcxx_AP_5::inCtrlAP) /*connect APContainer[i].outCtrlAP, APContainer[j].inCtrlAP*/;
			}
		}
		for (int j=0; j<(int)AppContainer.size();j++){
			APContainer[i].outCtrlApp_f.Connect(AppContainer[j],(compcxx_component::AP_outCtrlApp_f_t)&compcxx_Application_7::inCtrlAP) /*connect APContainer[i].outCtrlApp,AppContainer[j].inCtrlAP*/;
			AppContainer[j].outCtrlAP_f.Connect(APContainer[i],(compcxx_component::Application_outCtrlAP_f_t)&compcxx_AP_5::inCtrlApp) /*connect AppContainer[j].outCtrlAP,APContainer[i].inCtrlApp*/;
		}
	}
	Progress.p_compcxx_parent=this /*connect Progress.to_component,*/;
	Progress.Set(SimTime());
}


#line 84 "neko.cc"
void compcxx_Neko_9 :: Start(){

	
}


#line 89 "neko.cc"
void compcxx_Neko_9 :: Stop(){
	GenerateResultReport();
}


#line 93 "neko.cc"
void compcxx_Neko_9 :: GenerateRandom(){

	std::default_random_engine generator(rand());
	bool position;
	double Xaxis = 40;
	double Yaxis = 30;
	int numOfAPs = 3;
	int stationsPerAP = 25;
	std::vector<int> stations(numOfAPs,0.0);

	int numOfStations = 0;
	std::uniform_int_distribution<int> UStas(15, stationsPerAP);
	for (int i=0; i<(int)stations.size(); i++){
		stations.at(i) = UStas(generator);
		numOfStations += stations.at(i);
	}

	APContainer.SetSize(numOfAPs);
	STAContainer.SetSize(numOfStations);
	AppContainer.SetSize(numOfStations);

	int index = 0;
	for (int i=0; i<numOfAPs; i++){

		
		APContainer[i].apID = i;
		APContainer[i].configuration.TxPower = 20;
		APContainer[i].configuration.CCA = -82;
		APContainer[i].configuration.nSS = 2;
		APContainer[i].capabilities.IEEEProtocol = 2;
		APContainer[i].capabilities.Multilink = true;
		APContainer[i].capabilities.Mlearning = false;

		
		

		std::uniform_real_distribution<double> distX(0,Xaxis);
		std::uniform_real_distribution<double> distY(0,Yaxis);

		
		APContainer[i].coordinates.x = (i == 0) ? 10 : APContainer[i].coordinates.x = APContainer[i-1].coordinates.x + 10;
		
		APContainer[i].coordinates.y = 15;
		APContainer[i].coordinates.z = 3;

		
		position = false;
		while (!position){
			if (i == 0){
				position = true;
			}
			else{
				for (int n=0; n<i; n++){
					double distance = sqrt(pow((APContainer[n].coordinates.x - APContainer[i].coordinates.x),2) + pow((APContainer[n].coordinates.y - APContainer[i].coordinates.y),2) + pow((APContainer[n].coordinates.z - APContainer[i].coordinates.z),2));
					if (distance <= 5.0){
						APContainer[i].coordinates.x = distX(generator);
						APContainer[i].coordinates.y = distY(generator);
						n=0;
					}
					else{
						position = true;
					}
				}
			}
		}

		std::uniform_real_distribution<double> rad(1.0, 8.0); 
		for (int j=0; j<stations.at(i); j++){
			double r = rad(generator);
			double theta = 2*M_PI*drand48();
			AppContainer[index].destID = index;
			STAContainer[index].staID = index;
			STAContainer[index].servingAP = APContainer[i].apID;
			STAContainer[index].traffic_type = (Random()>0.8) ? "STREAMING" : "ELASTIC";
			
			STAContainer[index].coordinates.x = APContainer[i].coordinates.x + r*std::cos(theta);
			STAContainer[index].coordinates.y = APContainer[i].coordinates.y + r*std::sin(theta);
			STAContainer[index].coordinates.z = 1.5;
			STAContainer[index].configuration.TxPower = 15;
			STAContainer[index].configuration.CCA = -82;
			STAContainer[index].configuration.nSS = 2;
			STAContainer[index].capabilities.IEEEProtocol = 2;
			STAContainer[index].capabilities.Multilink = true;
			STAContainer[index].capabilities.Mlearning = false;

			WifiSTA station;
			station.id = index;
			station.coord.x = STAContainer[index].coordinates.x;
			station.coord.y = STAContainer[index].coordinates.y;
			station.coord.z = STAContainer[index].coordinates.z;
			station.traffic_type = STAContainer[index].traffic_type;
			APContainer[i].AssociatedSTAs.push_back(station);

			index++;
		}
	}
}


#line 191 "neko.cc"
void compcxx_Neko_9::GenerateResultReport(){

	
	if (channel_report){
		FILE* OccupancyReport = fopen("../Output/ChOccupancy.txt","a");
		fprintf(OccupancyReport, "*******************\n");
		fprintf(OccupancyReport, "Simultation seed: %i\n", seed);
		fprintf(OccupancyReport, "AP id; ChOcc 2_4; ChOcc5; ChOcc6\n");

		for (int i=0; i<(int)APContainer.size(); i++){
			fprintf(OccupancyReport, "%i; ", APContainer[i].apID);
			std::vector<double> AvgChOcc(maxIntNum, 0.0);
			for (int j=0; j<(int)APContainer[i].statistics.ChOcc.size(); j++){
				for (int n=0; n<(int)APContainer[i].statistics.ChOcc[j].size(); n++){
					if (APContainer[i].statistics.ChOcc[j].at(n) != -1){
						AvgChOcc.at(n) += APContainer[i].statistics.ChOcc[j].at(n);
					}
				}
			}
			for (int j=0; j<(int)AvgChOcc.size(); j++){
				if (AvgChOcc.at(j) != 0){
					AvgChOcc.at(j) = AvgChOcc.at(j)/(int)APContainer[i].statistics.SimT.size();
				}
				if(j<(int)AvgChOcc.size()-1){
					fprintf(OccupancyReport, "%f; ", AvgChOcc.at(j));
				}
				else{
					fprintf(OccupancyReport, "%f", AvgChOcc.at(j));
				}
			}
			fprintf(OccupancyReport, "\n");
		}
	}
	if (stats_report){
		FILE* StatsReport = fopen("../Output/Stats.txt","a");
		fprintf(StatsReport, "*******************\n");
		fprintf(StatsReport, "Simultation seed: %i\n", seed);
		fprintf(StatsReport, "Avg_Sat; Avg_Th; Avg_Dratio\n");

		double TotalSatAvg = 0, TotalThAvg = 0;
		for (int i=0; i<(int)STAContainer.size(); i++){
			TotalSatAvg += std::accumulate(STAContainer[i].statistics.AvgSatPerFlow.begin(), STAContainer[i].statistics.AvgSatPerFlow.end(),0.0)/(int)STAContainer[i].statistics.AvgSatPerFlow.size();
			TotalThAvg += std::accumulate(STAContainer[i].statistics.AvgThPerFlow.begin(), STAContainer[i].statistics.AvgThPerFlow.end(),0.0)/(int)STAContainer[i].statistics.AvgThPerFlow.size();
		}
		fprintf(StatsReport, "%f; %f; ", TotalSatAvg/(int)STAContainer.size(), TotalThAvg);

		double TotalAvgDrop = 0;
		for (int i=0; i<(int)APContainer.size(); i++){
			TotalAvgDrop += std::accumulate(APContainer[i].statistics.AvgDRPerFlow.begin(), APContainer[i].statistics.AvgDRPerFlow.end(),0.0)/(double)APContainer[i].statistics.AvgDRPerFlow.size();
		}
		fprintf(StatsReport, "%f", TotalAvgDrop/(int)APContainer.size());
		fprintf(StatsReport, "\n");
	}
}


#line 246 "neko.cc"
void compcxx_Neko_9::ProgressBar(trigger_t&){

	int now = (int)SimTime();
	double t = runTime;
	double progress = now/t;

	int barWidth = 50;
	int pos = barWidth * progress;
	std::cout << "[";

	for (int i = 0; i < barWidth; i++) {
				if (i < pos) std::cout << "=";
				else if (i == pos) std::cout << ">";
				else std::cout << " ";
		}

	std::cout << "] " << int(progress * 100.0) << "%\r";
	std::cout.flush();

	if (progress < 100){
		Progress.Set(SimTime()+9);
	}
}


#line 270 "neko.cc"
int main(int argc, char *argv[]){

	std::cout << "************* SIMULATION STARTS *************" << std::endl;
	compcxx_Neko_9 test;

	if (rnd == 1) {
		test.seed = atoi(argv[1]);
		medBW = atof(argv[2]);
		policy = argv[3];
		srand(test.seed);
	}
	else{
		test.seed=time(NULL);
		srand(test.seed);
	}

	test.StopTime(runTime);
	test.Setup();
	test.Run();

	std::cout << "*********************************************" << std::endl;
}
