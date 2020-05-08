
#line 1 "neko.cc"
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include <string>


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







#line 11 "neko.cc"


#line 1 "../Main/macros.h"






const int runTimeSim = 86400;           
const int rnd = 1;                      
const int lowBW = 2;                    
const int medBW = 5;                    
const int highBW = 8;                   
const int propagation = 1;              
const int LearningWindowTime = 540;     
const double ProbUserAgentEnabled = 1;  




      const int frameLength = 12000;                    

      const int Lsf = 16;                               
      const int Lmac = 320;                             
      const int Ltb = 18;                               
      const int Lack = 112;                             
      const int Lrts = 160;                             
      const int Lcts = 112;                             
      const int CW = 16;                                

      const double Tempty = 9*pow(10,-6);               
      const double Tsifs = 16*pow(10,-6);               
      const double Tdifs = 34*pow(10,-6);               
      const double TphyL = 20*pow(10,-6);               
      const double TphyHE = 164*pow(10,-6);             
      const double Tofdm_leg = 4*pow(10,-6);            
      const double Tofdm = 16*pow(10,-6);               
      const double legacyRate = 24;                     

      const double Pe = 0.1;                            




































const int stationLearningFlag = 2;      
const int APlearningFlag = 2;           




const double flowDuration = 20;            
const double flowActivation = 60;          



const int SearchBestAP = 180;           
const int ChSelectionTime = 180;        

const int offsetAP = 7200;              
const int offsetSTA = 7200;             

#line 12 "neko.cc"


#line 1 "../Methods/freq_and_modulation.h"
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

double *GetConfiguration (int IEEEprotocol, int action){

double *configuration = new double[3];

  switch (IEEEprotocol) {
    case 0:{

      int actions [] = {0,1,2,3,4};
      int size = sizeof(actions)/sizeof(int);
      int Channel [] = {1,6,11,3,11};
      double fc []  = {2.412,2.437,2.462,2.422,2.462};

      if (action <= 2){
        configuration [0] = 20;

        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
      else{
        configuration [0] = 40;

        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
    }break;

    case 1:{

      




      int actions [] = {0,1,2,3,4,5,6};
      int size = sizeof(actions)/sizeof(int);
      int Channel [] = {36,40,44,48,38,46,42};
      double fc []  = {5.18,5.2,5.22,5.24,5.19,5.23,5.21};

      if (action <= 3){ 
        configuration [0] = 20;

        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
      else if((4 == action) || (action == 5)){ 
        configuration [0] = 40;

        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
      else if (action == 6){ 
        configuration [0] = 80;
        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
    }break;

    case 2:{ 

      int actions [] = {0,1,2,3,4,5,6};
      int size = sizeof(actions)/sizeof(int);
      int Channel [] = {36,40,44,48,38,46,42};
      double fc []  = {5.18,5.2,5.22,5.24,5.19,5.23,5.21};

      if (action <= 3){ 
        configuration [0] = 20;

        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
      else if((4 == action) || (action == 5)){ 
        configuration [0] = 40;

        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
      else if (action == 6){ 
        configuration [0] = 80;
        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
    }break;
  }
  return configuration;
}

int SelectMCS (double Prx, int protocol){

  int MCS_selected;

  switch (protocol) {
    case 0:{
      if (Prx < -82) {MCS_selected = -1;} 
      else if (Prx >= -82 && Prx < -79) {MCS_selected = 0;}
      else if (Prx >= -79 && Prx < -77) {MCS_selected = 1;}
      else if (Prx >= -77 && Prx < -74) {MCS_selected = 2;}
      else if (Prx >= -74 && Prx < -70) {MCS_selected = 3;}
      else if (Prx >= -70 && Prx < -66) {MCS_selected = 4;}
      else if (Prx >= -66 && Prx < -65) {MCS_selected = 5;}
      else if (Prx >= -65 && Prx < -64) {MCS_selected = 6;}
      else if (Prx >= -64 && Prx < -59) {MCS_selected = 7;}
      else if (Prx >= -59) {MCS_selected = 7;}
    }break;

    case 1:{
      if (Prx < -82) {MCS_selected = -1;} 
      else if (Prx >= -82 && Prx < -79) {MCS_selected = 0;}
      else if (Prx >= -79 && Prx < -77) {MCS_selected = 1;}
      else if (Prx >= -77 && Prx < -74) {MCS_selected = 2;}
      else if (Prx >= -74 && Prx < -70) {MCS_selected = 3;}
      else if (Prx >= -70 && Prx < -66) {MCS_selected = 4;}
      else if (Prx >= -66 && Prx < -65) {MCS_selected = 5;}
      else if (Prx >= -65 && Prx < -64) {MCS_selected = 6;}
      else if (Prx >= -64 && Prx < -59) {MCS_selected = 7;}
      else if (Prx >= -59 && Prx < -57) {MCS_selected = 8;}
      else if (Prx >= -57) {MCS_selected = 9;}
    }break;

    case 2:{
      if (Prx < -82) {MCS_selected = -1;} 
      else if (Prx >= -82 && Prx < -79) {MCS_selected = 0;}
      else if (Prx >= -79 && Prx < -77) {MCS_selected = 1;}
      else if (Prx >= -77 && Prx < -74) {MCS_selected = 2;}
      else if (Prx >= -74 && Prx < -70) {MCS_selected = 3;}
      else if (Prx >= -70 && Prx < -66) {MCS_selected = 4;}
      else if (Prx >= -66 && Prx < -65) {MCS_selected = 5;}
      else if (Prx >= -65 && Prx < -64) {MCS_selected = 6;}
      else if (Prx >= -64 && Prx < -59) {MCS_selected = 7;}
      else if (Prx >= -59 && Prx < -57) {MCS_selected = 8;}
      else if (Prx >= -57 && Prx < -54) {MCS_selected = 9;}
      else if (Prx >= -54 && Prx < -52) {MCS_selected = 10;}
      else if (Prx >= -52) {MCS_selected = 11;}
    }break;
  }

  return MCS_selected;
}

double SetDataRate(double RSSI, int IEEEprotocol, int CH_Bandwidth){

  double data_rate;
  int numDataSubcarriers;

  switch (IEEEprotocol) {
    case 0:{
      switch (CH_Bandwidth) {
        case 20:{
          
          numDataSubcarriers = 52;
        }break;

        case 40:{
          
          numDataSubcarriers = 108;
        }break;
      }

      int MCS_index = SelectMCS(RSSI, IEEEprotocol);
      int BitsSimbol_Modulation [] = {1,2,2,4,4,6,6,6};  
      double codingR [] = {1/double(2),1/double(2),3/double(4),1/double(2),3/double(4),2/double(3),3/double(4),5/double(6)}; 
      int Nss = 2;              

      if (MCS_index == -1){
        data_rate = 0;
      }
      else{
        int modBits = BitsSimbol_Modulation [MCS_index];
        double codingRate = codingR [MCS_index];
        data_rate = (Nss * modBits * codingRate * numDataSubcarriers * 1/Tofdm) /1000000; 
      }
    }break;

    case 1:{
      switch (CH_Bandwidth) {
        case 20:{
          
          numDataSubcarriers = 52;
        }break;

        case 40:{
          
          numDataSubcarriers = 108;
        }break;

        case 80:{
          
          numDataSubcarriers = 234;
        }break;
      }

      int MCS_index = SelectMCS(RSSI, IEEEprotocol);
      int BitsSimbol_Modulation [] = {1,2,2,4,4,6,6,6,8,8};  
      double codingR [] = {1/double(2),1/double(2),3/double(4),1/double(2),3/double(4),2/double(3),3/double(4),5/double(6),3/double(4),5/double(6)}; 
      int Nss = 2;              

      if (MCS_index == -1){
        data_rate = 0;
      }
      else{
        int modBits = BitsSimbol_Modulation [MCS_index];
        double codingRate = codingR [MCS_index];
        data_rate = (Nss * modBits * codingRate * numDataSubcarriers * 1/Tofdm) /1000000; 
      }

    }break;

    case 2:{
      switch (CH_Bandwidth) {
        case 20:{
          
          numDataSubcarriers = 234;
        }break;

        case 40:{
          
          numDataSubcarriers = 468;
        }break;

        case 80:{
          
          numDataSubcarriers = 980;
        }break;
      }

      int MCS_index = SelectMCS(RSSI, IEEEprotocol);
      int BitsSimbol_Modulation [] = {1,2,2,4,4,6,6,6,8,8,10,10};  
      double codingR [] = {1/double(2),1/double(2),3/double(4),1/double(2),3/double(4),2/double(3),3/double(4),5/double(6),3/double(4),5/double(6),5/double(6),5/double(6)}; 
      int Nss = 2;              

      if (MCS_index == -1){
        data_rate = 0;
      }
      else{
        int modBits = BitsSimbol_Modulation [MCS_index];
        double codingRate = codingR [MCS_index];
        data_rate = (Nss * modBits * codingRate * numDataSubcarriers * 1/Tofdm) /1000000; 
      }

    }break;
  }

  return data_rate;
}

#line 13 "neko.cc"


#line 1 "../Methods/pathloss.h"
#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>


double PropL(double clientX, double clientY, double clientZ, double ApX, double ApY, double ApZ, double f){

  switch (propagation) {

    
    case 0:{
      int n_walls = 5;                                         
      int n_floors = 2;                                        
      double dBP = 5;                                         
      double expo;
      double dBP_losses, propagationLosses;
      double distance;                                          

      distance = sqrt(pow(ApX-clientX, 2)+pow(ApY-clientY, 2)+pow(ApZ-clientZ, 2));

      if (distance >= dBP){
        dBP_losses = 35*log10(distance/dBP);
      }
      else{
        dBP_losses = 0;
      }

      expo=((distance/n_floors)+2)/((distance/n_floors)+1)-0.46;

      propagationLosses = 40.05 + 20*log10(f/2.4) + 20*log10(std::min(distance,dBP)) + dBP_losses + 18.3*pow((distance/n_floors),expo)+ 5*(distance/n_walls);

      return propagationLosses;

    }break;

    
    case 1:{

      int n_walls = 4;                                         
      double dBP = 5;                                         
      double dBP_losses, propagationLosses;
      double distance;                                         

      distance = sqrt(pow(ApX-clientX, 2)+pow(ApY-clientY, 2)+pow(ApZ-clientZ, 2));

      if (distance >= dBP){
        dBP_losses = 35*log10(distance/dBP);
      }
      else{
        dBP_losses = 0;
      }

      propagationLosses = 40.05 + 20*log10(f/2.4) + 20*log10(std::min(distance,dBP)) + dBP_losses + 7*n_walls;

      return propagationLosses;

    }break;

    
    case 2:{



    }break;
  }
}

#line 14 "neko.cc"


#line 1 "../Methods/helpers.h"

double GetData (int flow, int initVal, int size, std::vector<double>* s, double Bw){
  double bits;
  switch (flow) {
    case 0:{
      double sat, avg_sat;
      int count = 0;
      sat = 0;
      avg_sat = 0;

      for (int i=0; i<size; i++){
        sat = sat + s->at(initVal);
        count++;
        initVal++;
      }

      avg_sat = sat/count;
      bits = Bw*avg_sat;

      
      break;
    }
    case 1:{
      break;
    }
  }
  return bits;
}

int SearchAction (int action, int numOfarms, std::vector<int>* setOfactions){

  int index, value;

  for (int i=0; i<numOfarms; i++){
    value = setOfactions->at(i);
    if (value == action){
      index = i;
    }
  }
  return index;
}

double GetReward (int currentAction, std::vector<double>* s, std::vector<int>* v, std::vector<double>* tS, double t){

  int times, v_value, size;
  double r, avgReward, lowerBound, upperBound, s_value;

  size = tS->size();
  r = 0;
  times = 0;

  if (t-LearningWindowTime < 0){
    lowerBound = 0;
    upperBound = t;
  }
  else{
    lowerBound = t-LearningWindowTime;
    upperBound = t;
  }

  for (int i=0; i<size; i++){
    if ((lowerBound <= tS->at(i)) && (tS->at(i) < upperBound)){
      v_value = v->at(i);
      if (v_value == currentAction){
        s_value = s->at(i);
        r = r + s_value;
        times++;
      }
    }
  }

  if (r == 0){
    avgReward = 0;
  }
  else{
      avgReward = r/times;
  }
  
  return avgReward;
}

double GetOccupancy (int currentAction, std::vector<double>* o, std::vector<int>* v, std::vector<double>* tS, double t){

  int times, v_value, size;
  double r, avgOccupancy, lowerBound, upperBound, o_value;

  size = tS->size();
  r = 0;
  times = 0;

  if (t-LearningWindowTime < 0){
    lowerBound = 0;
    upperBound = t;
  }
  else{
    lowerBound = t-LearningWindowTime;
    upperBound = t;
  }

  for (int i=0; i<size; i++){
    if ((lowerBound <= tS->at(i)) && (tS->at(i) < upperBound)){
      v_value = v->at(i);
      if (v_value == currentAction){
        o_value = o->at(i);
        r = r + o_value;
        times++;
      }
    }
  }

  if (r == 0){
    avgOccupancy = 0;
  }
  else{
      avgOccupancy = r/times;
  }
  
  return avgOccupancy;
}

int ChannelOverlappingDetector (int protocol, int Ch_1, int Ch_2){

  int Ch_overlapped;

  switch (protocol) {
    case 0:{
      
    }break;

    case 1:{

      switch (Ch_1) {
        case 36:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 38)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 40:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 38)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 44:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 46)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 48:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 46)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        































        case 38:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 36)||(Ch_2 == 40)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 46:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 44)||(Ch_2 == 48)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        















        case 42:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 36)||(Ch_2 == 38)||(Ch_2 == 40)||(Ch_2 == 44)||(Ch_2 == 46)||(Ch_2 == 48)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
      







      }
    }break;

    case 2:{

      switch (Ch_1) {
        case 36:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 38)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 40:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 38)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 44:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 46)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 48:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 46)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        































        case 38:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 36)||(Ch_2 == 40)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 46:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 44)||(Ch_2 == 48)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        















        case 42:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 36)||(Ch_2 == 38)||(Ch_2 == 40)||(Ch_2 == 44)||(Ch_2 == 46)||(Ch_2 == 48)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
      







      }
    }break;

  }
  return Ch_overlapped;
}

#line 15 "neko.cc"


#line 1 "../Learning/strategies.h"
#include <math.h>
#include <random>



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


int ThompsonSampling (int num_actions, std::vector<double>* estimated_reward, std::vector<double>* occupancy, std::vector<int>* times_arm_selected) {

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

	times_arm_selected->at(action_ix) = times_arm_selected->at(action_ix) + 1;
	return action_ix;
}

#line 16 "neko.cc"


#line 1 "../Structures/notifications.h"


struct Header{

  int sourceID;
  int destinationID;
  double X,Y,Z;
};

struct APBeacon{

  Header header;

  double Tx_Power;
  double freq;
  double Load;
  int Channel;
  int protocolType;
  int BW;

  





};



struct ApNotification{

  Header header;

  int ChannelNumber;
  double Load;
  int flag;
};



struct StationInfo{

  Header header;

  double RSSI;
  double AirTime;
};

struct Connection{

  Header header;

  double LoadByStation;
  double Ap_Load;
};

#line 17 "neko.cc"


#line 1 "ap.h"
#ifndef _AP_
#define _AP_

#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <vector>
#include <string>


#line 645 "ap.h"
#endif

#line 18 "neko.cc"


#line 1 "station.h"
#ifndef _STA_
#define _STA_


#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <vector>
#include <time.h>
#include <deque>



#line 507 "station.h"
#endif

#line 19 "neko.cc"


#include "compcxx_neko.h"
class compcxx_AP_9;/*template <class T> */
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
public:compcxx_AP_9* p_compcxx_parent;};

class compcxx_AP_9;/*template <class T> */
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
public:compcxx_AP_9* p_compcxx_parent;};

class compcxx_AP_9;/*template <class T> */
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
public:compcxx_AP_9* p_compcxx_parent;};


#line 13 "ap.h"
class compcxx_AP_9 : public compcxx_component, public TypeII {

public:

  
  int apID;                                                     
  double X, Y, Z;                                               
  int numOfConnectedStations;                                   
  double trafficLoad;                                           

  int actionSelected;
  int channelBW;                                                
  int txPower;                                                  
  int OperatingChannel;                                         
  double frequency;                                             
  double CCA;                                                   

  int IEEEprotocolType;                                         
  std::vector<int> CHMapToAction;                               
  std::vector<int> setOfactions;                                

  
  std::vector<int> vectorOfConnectedStations;                   
  std::vector<double> vectorOfConnectedStationsRSSIs;           

  std::vector<int> vectorOfNeighboringAPs;                      
  std::vector<double> vectorOfNeighboringRSSIs;                 

  std::vector<double> CH_occupancy_detected;                    

  
  std::vector<double> reward_action;                            
  std::vector<double> occupancy_CH;                             
  std::vector<double> estimated_reward_action;
  std::vector<int> TimesActionIsPicked;                               
  std::vector< std::vector<double> > estimated_reward_Per_action;    
  std::vector< std::vector<double> > estimated_reward_Per_action_Time;

  
  std::vector<int> Action_Selected;                             
  std::vector<double> channel_reward;                           
  std::vector<double> occupanyOfAp;                             
  std::vector<double> TimeSimulation;

  std::vector<int> ActionChange;
  std::vector<double> TimeStamp;

  double iter;
  int flag, isolation;

public:

  compcxx_AP_9();

  
  void Setup();
  void Start();
  void Stop();

  
  /*inport */void inSetNeighbors(APBeacon &b);
  /*inport */void inSetClientAssociation(StationInfo &i);
  /*inport */void inRequestedAirTime(Connection &n);
  /*inport */void inTxTimeFinished(Connection &n);
  /*inport */void inLoadFromNeighbor(ApNotification &ap);
  /*inport */void inUpdateConnection(StationInfo &i, int k);
  /*inport */void inUpdateAttachedStationsParams(StationInfo &i);

  
  class my_AP_outSetNeighbors_f_t:public compcxx_functor<AP_outSetNeighbors_f_t>{ public:void  operator() (APBeacon &b) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(b); return (c[0]->*f[0])(b);};};my_AP_outSetNeighbors_f_t outSetNeighbors_f;/*outport void outSetNeighbors(APBeacon &b)*/;
  class my_AP_outSendBeaconToNodes_f_t:public compcxx_functor<AP_outSendBeaconToNodes_f_t>{ public:void  operator() (APBeacon &b) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(b); return (c[0]->*f[0])(b);};};my_AP_outSendBeaconToNodes_f_t outSendBeaconToNodes_f;/*outport void outSendBeaconToNodes(APBeacon &b)*/;
  class my_AP_outAssignAirTime_f_t:public compcxx_functor<AP_outAssignAirTime_f_t>{ public:void  operator() (Connection &n) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(n); return (c[0]->*f[0])(n);};};my_AP_outAssignAirTime_f_t outAssignAirTime_f;/*outport void outAssignAirTime(Connection &n)*/;
  class my_AP_outLoadToNeighbor_f_t:public compcxx_functor<AP_outLoadToNeighbor_f_t>{ public:void  operator() (ApNotification &ap) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(ap); return (c[0]->*f[0])(ap);};};my_AP_outLoadToNeighbor_f_t outLoadToNeighbor_f;/*outport void outLoadToNeighbor(ApNotification &ap)*/;
  class my_AP_outChannelChange_f_t:public compcxx_functor<AP_outChannelChange_f_t>{ public:void  operator() (APBeacon &b) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(b); return (c[0]->*f[0])(b);};};my_AP_outChannelChange_f_t outChannelChange_f;/*outport void outChannelChange(APBeacon &b)*/;

  
  compcxx_Timer_2 /*<trigger_t> */trigger_Action;
  compcxx_Timer_3 /*<trigger_t> */trigger_APBootUp;
  compcxx_Timer_4 /*<trigger_t> */trigger_progress;

  /*inport */inline void CHselectionBylearning(trigger_t&);
  /*inport */inline void APBootUp(trigger_t&);
  /*inport */inline void ProgressFunct(trigger_t&);
};

class compcxx_Station_10;/*template <class T> */
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
public:compcxx_Station_10* p_compcxx_parent;};

class compcxx_Station_10;/*template <class T> */
#line 267 "../COST/cost.h"
class compcxx_Timer_5 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_5() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
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
public:compcxx_Station_10* p_compcxx_parent;};

class compcxx_Station_10;/*template <class T> */
#line 267 "../COST/cost.h"
class compcxx_Timer_6 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_6() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
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
public:compcxx_Station_10* p_compcxx_parent;};

class compcxx_Station_10;/*template <class T> */
#line 267 "../COST/cost.h"
class compcxx_Timer_7 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_7() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
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
public:compcxx_Station_10* p_compcxx_parent;};


#line 16 "station.h"
class compcxx_Station_10 : public compcxx_component, public TypeII {

public:
  int staID;                                                        
  int userType;                                                     
  int servingAP;                                                    
  double X, Y, Z;                                                   

  int txPower;                                                      
  double DLDataRate;                                                
  double ULDataRate;                                                
  double RSSI;                                                      
  int ChBW;                                                         
  double frequency;                                                 
  int IEEE_protocol;                                                

  double AirTimeRequired;                                           
  int FlowType;                                                     

  std::deque <APBeacon> detectedWLANs;                              
  std::vector<int> InRangeAPs;                                      
  std::vector<int> CandidateAPs;                                    


  
  std::vector<double> AirTimeEvo;                                   
  std::vector<double> Satisfaction;                                 
  std::vector<double> CandidateAPsTrafficLoad;                      

  std::vector<double> reward_action;                                
  std::vector<double> estimated_reward_action;
  std::vector<double> occupancy_AP;                                 

  std::vector<int> Times_ActionSelected;                            
  std::vector<int> Action_Selected;                                 
  std::vector<int> ActionChange;
  std::vector<double> TimeStamp;
  std::vector<double> Throughput;

  std::vector< std::vector<double> > estimated_reward_Per_action;    
  std::vector< std::vector<double> > estimated_reward_Per_action_Time;

  std::vector<double> mSat;
  std::vector<double> timeSim;                                      
  std::vector<double> timeSim2;                                     

  
  double iter;
  int flag, TimeSizeF, TimeSizeS;
  double requestedBW, startTX, finishTX, bits_Sent, totalBits, timeActive;

public:

  compcxx_Station_10();

  
  void Setup();
  void Start();
  void Stop();

  
  /*inport */void inReceivedBeacon(APBeacon &b);
  /*inport */void inAssignedAirTime(Connection &n);
  /*inport */void inUpdateStationParameters(APBeacon &b);

  
  class my_Station_outSetClientAssociation_f_t:public compcxx_functor<Station_outSetClientAssociation_f_t>{ public:void  operator() (StationInfo &i) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(i); return (c[0]->*f[0])(i);};};my_Station_outSetClientAssociation_f_t outSetClientAssociation_f;/*outport void outSetClientAssociation(StationInfo &i)*/;
  class my_Station_outRequestAirTime_f_t:public compcxx_functor<Station_outRequestAirTime_f_t>{ public:void  operator() (Connection &n) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(n); return (c[0]->*f[0])(n);};};my_Station_outRequestAirTime_f_t outRequestAirTime_f;/*outport void outRequestAirTime(Connection &n)*/;
  class my_Station_outFlowEnded_f_t:public compcxx_functor<Station_outFlowEnded_f_t>{ public:void  operator() (Connection &n) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(n); return (c[0]->*f[0])(n);};};my_Station_outFlowEnded_f_t outFlowEnded_f;/*outport void outFlowEnded(Connection &n)*/;
  class my_Station_outUpdateAttachedStationsParams_f_t:public compcxx_functor<Station_outUpdateAttachedStationsParams_f_t>{ public:void  operator() (StationInfo &i) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(i); return (c[0]->*f[0])(i);};};my_Station_outUpdateAttachedStationsParams_f_t outUpdateAttachedStationsParams_f;/*outport void outUpdateAttachedStationsParams(StationInfo &i)*/;
  class my_Station_outUpdateConnection_f_t:public compcxx_functor<Station_outUpdateConnection_f_t>{ public:void  operator() (StationInfo &i, int k) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(i,k); return (c[0]->*f[0])(i,k);};};my_Station_outUpdateConnection_f_t outUpdateConnection_f;/*outport void outUpdateConnection(StationInfo &i, int k)*/;

  
  compcxx_Timer_5 /*<trigger_t> */trigger_ProcessBeacons;
  compcxx_Timer_6 /*<trigger_t> */trigger_SendRequestedAT;
  compcxx_Timer_7 /*<trigger_t> */trigger_TxTimeFinished;
  compcxx_Timer_8 /*<trigger_t> */trigger_Action;

  /*inport */inline void ProcessBeacons(trigger_t&);
  /*inport */inline void SendRequestedAT(trigger_t&);
  /*inport */inline void SendTxTimeFinished(trigger_t&);
  /*inport */inline void APselectionBylearning(trigger_t&);

};


#line 21 "neko.cc"
class compcxx_Neko_11 : public compcxx_component, public CostSimEng {

public:

  
  void Setup();
  void Start();
  void Stop();

  
  void GenerateRandom();
  void LoadScenario();

public:

  compcxx_array<compcxx_AP_9  >APoint_container;
  compcxx_array<compcxx_Station_10  >STA_container;

  int seed;

  int numOfAPs;
  int numOfStations;
};


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
  (p_compcxx_parent->APBootUp(m_event.data));
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
  (p_compcxx_parent->CHselectionBylearning(m_event.data));
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
  (p_compcxx_parent->ProgressFunct(m_event.data));
}




#line 93 "ap.h"

#line 94 "ap.h"

#line 95 "ap.h"

#line 98 "ap.h"
compcxx_AP_9 :: compcxx_AP_9(){
  trigger_Action.p_compcxx_parent=this /*connect trigger_Action.to_component,*/;
  trigger_APBootUp.p_compcxx_parent=this /*connect trigger_APBootUp.to_component,*/;
  trigger_progress.p_compcxx_parent=this /*connect trigger_progress.to_component,*/;
}


#line 104 "ap.h"
void compcxx_AP_9 :: Setup(){

}


#line 108 "ap.h"
void compcxx_AP_9 :: Start(){

  switch (IEEEprotocolType) {
    case 0:{
      int actions [] = {0,1,2};
      int Channel [] = {1,6,11};
      int size = sizeof(actions)/sizeof(int);

      for (int i=0; i<size; i++){
        setOfactions.push_back(actions[i]);
        CHMapToAction.push_back(Channel[i]);
      }
      TimesActionIsPicked.assign(size,0.0);
      CH_occupancy_detected.assign(size, 0.0);
      trigger_APBootUp.Set(0);

    }break;

    case 1:{

      int actions [] = {0,1,2,3};
      int Channel [] = {36,40,44,48};
      int size = sizeof(actions)/sizeof(int);

      for (int i=0; i<size; i++){
        setOfactions.push_back(actions[i]);
        CHMapToAction.push_back(Channel[i]);
      }
      TimesActionIsPicked.assign(size,0.0);
      CH_occupancy_detected.assign(size, 0.0);
      trigger_APBootUp.Set(0);

    }break;

    case 2:{

      int actions [] = {0,1,2};
      int Channel [] = {36,40,44};
      int size = sizeof(actions)/sizeof(int);

      for (int i=0; i<size; i++){
        setOfactions.push_back(actions[i]);
        CHMapToAction.push_back(Channel[i]);
      }
      TimesActionIsPicked.assign(size,0.0);
      CH_occupancy_detected.assign(size, 0.0);

      trigger_APBootUp.Set(0);

    }break;
  }

  if (apID == 0) {
    trigger_progress.Set(4320);
  }

  iter = 1;
  flag = 0;
  isolation = 0;
}


#line 169 "ap.h"
void compcxx_AP_9 :: Stop(){

}


#line 173 "ap.h"
void compcxx_AP_9 :: APBootUp(trigger_t&){

  double *selectConfiguration = GetConfiguration(IEEEprotocolType, actionSelected);

  channelBW = *selectConfiguration;
  OperatingChannel = *(selectConfiguration+1);
  frequency = *(selectConfiguration+2);

  APBeacon beacon;

  beacon.header.sourceID = apID;
  beacon.header.X = X;
  beacon.header.Y = Y;
  beacon.header.Z = Z;
  beacon.Tx_Power = txPower;
  beacon.freq = frequency;
  beacon.protocolType = IEEEprotocolType;
  beacon.BW = channelBW;

  if (isolation == 0) {
    (outSetNeighbors_f(beacon));
  }

  (outSendBeaconToNodes_f(beacon));

  ActionChange.push_back(actionSelected);
  TimeStamp.push_back(SimTime());

  switch (APlearningFlag) {
    case 0:{
      
    }break;

    case 1:{

      int size = setOfactions.size();

      reward_action.assign(size,0.0);
      occupancy_CH.assign(size,0.0);

      trigger_Action.Set(SimTime()+Exponential(2)+offsetAP);

    }break;

    case 2:{

      int size = setOfactions.size();

      reward_action.assign(size,0.0);
      occupancy_CH.assign(size,0.0);
      estimated_reward_action.assign(size,0.0);
      estimated_reward_Per_action.resize(size);
      estimated_reward_Per_action_Time.resize(size);

      trigger_Action.Set(SimTime()+Exponential(2)+offsetAP);

    }break;
  }
}


#line 233 "ap.h"
void compcxx_AP_9 :: inSetNeighbors (APBeacon &b){

  double RSSIvalue;
  RSSIvalue = txPower - PropL(b.header.X,b.header.Y,b.header.Z,X,Y,Z,b.freq);

  if (RSSIvalue>=CCA){
    vectorOfNeighboringAPs.push_back(b.header.sourceID);
    vectorOfNeighboringRSSIs.push_back(RSSIvalue);
  }
}


#line 244 "ap.h"
void compcxx_AP_9 :: inSetClientAssociation (StationInfo &s){

  if (s.header.destinationID == apID){

    numOfConnectedStations++;
    vectorOfConnectedStations.push_back(s.header.sourceID);
    vectorOfConnectedStationsRSSIs.push_back(s.RSSI);
  }
}


#line 254 "ap.h"
void compcxx_AP_9 :: inRequestedAirTime(Connection &STARequest){

  if (apID == STARequest.header.destinationID){
    trafficLoad = trafficLoad + STARequest.LoadByStation;

    Connection APResponse;
    APResponse.header.sourceID = apID;
    APResponse.Ap_Load = trafficLoad;

    if ((int)vectorOfConnectedStations.size() != 0){
      for (int i=0;i<(int)vectorOfConnectedStations.size();i++){
        APResponse.header.destinationID = vectorOfConnectedStations.at(i);
        (outAssignAirTime_f(APResponse));
      }
    }

    ApNotification AddLoad;
    AddLoad.Load = STARequest.LoadByStation;
    AddLoad.ChannelNumber = OperatingChannel;
    AddLoad.flag = 1;

    if ((int)vectorOfNeighboringAPs.size() != 0){
      for (int i=0;i<(int)vectorOfNeighboringAPs.size();i++){
        AddLoad.header.destinationID = vectorOfNeighboringAPs.at(i);
        (outLoadToNeighbor_f(AddLoad));
      }
    }

    channel_reward.push_back(std::max(0.0,1-(double)(trafficLoad/100)));
    Action_Selected.push_back(actionSelected);

    if (trafficLoad < 100){
      occupanyOfAp.push_back(trafficLoad);
      TimeSimulation.push_back(SimTime());
    }
    else{
      occupanyOfAp.push_back((double)100);
      TimeSimulation.push_back(SimTime());
    }
  }
}


#line 296 "ap.h"
void compcxx_AP_9 :: inTxTimeFinished(Connection &EndConn){

  if (apID == EndConn.header.destinationID){
    trafficLoad = trafficLoad - EndConn.LoadByStation;

    if (trafficLoad < 0.00001){
      trafficLoad = 0;
    }

    Connection APResponse;
    APResponse.Ap_Load = trafficLoad;
    APResponse.header.sourceID = apID;

    if ((int)vectorOfConnectedStations.size() != 0){
      for (int i=0;i<(int)vectorOfConnectedStations.size();i++){
        if (vectorOfConnectedStations.at(i) !=EndConn.header.sourceID){
          APResponse.header.destinationID = vectorOfConnectedStations.at(i);
          (outAssignAirTime_f(APResponse));
        }
      }
    }

    ApNotification RemoveLoad;
    RemoveLoad.Load = EndConn.LoadByStation;
    RemoveLoad.ChannelNumber = OperatingChannel;
    RemoveLoad.flag = 0;

    if ((int)vectorOfNeighboringAPs.size() != 0){
      for (int i=0;i<(int)vectorOfNeighboringAPs.size();i++){
        RemoveLoad.header.destinationID = vectorOfNeighboringAPs.at(i);
        (outLoadToNeighbor_f(RemoveLoad));
      }
    }

    channel_reward.push_back(std::max(0.0,1-(double)(trafficLoad/100)));
    Action_Selected.push_back(actionSelected);

    if (trafficLoad < 100){
      occupanyOfAp.push_back(trafficLoad);
      TimeSimulation.push_back(SimTime());
    }
    else{
      occupanyOfAp.push_back((double)100);
      TimeSimulation.push_back(SimTime());
    }
  }
}


#line 344 "ap.h"
void compcxx_AP_9 :: inLoadFromNeighbor(ApNotification &notification){

  if (apID == notification.header.destinationID){

    int Neighbor_CH_index;
    for (int i=0; i<(int)CHMapToAction.size(); i++){
      if (notification.ChannelNumber == CHMapToAction.at(i)){
        Neighbor_CH_index = i;
      }
    }

    int overlapping = ChannelOverlappingDetector(IEEEprotocolType,OperatingChannel,notification.ChannelNumber);

    if (notification.flag == 1){
      if (overlapping == 1){
        CH_occupancy_detected.at(Neighbor_CH_index) = CH_occupancy_detected.at(Neighbor_CH_index)+notification.Load;
        trafficLoad = trafficLoad + notification.Load;
      }
      else{
        CH_occupancy_detected.at(Neighbor_CH_index) = CH_occupancy_detected.at(Neighbor_CH_index)+notification.Load;
      }
    }
    if (notification.flag == 0){
      if (overlapping == 1){
        trafficLoad = trafficLoad - notification.Load;
        if (trafficLoad < 0.00001){
          trafficLoad = 0;
        }
        CH_occupancy_detected.at(Neighbor_CH_index) = CH_occupancy_detected.at(Neighbor_CH_index)-notification.Load;
        if (CH_occupancy_detected.at(Neighbor_CH_index) < 0.00001){
          CH_occupancy_detected.at(Neighbor_CH_index) = 0;
        }
      }
      else{
        CH_occupancy_detected.at(Neighbor_CH_index) = CH_occupancy_detected.at(Neighbor_CH_index)-notification.Load;
        if (CH_occupancy_detected.at(Neighbor_CH_index) < 0.00001){
          CH_occupancy_detected.at(Neighbor_CH_index) = 0;
        }
      }
    }


    channel_reward.push_back(std::max(0.0,1-(double)(trafficLoad/100)));
    Action_Selected.push_back(actionSelected);

    if (trafficLoad < 100){
      occupanyOfAp.push_back(trafficLoad);
      TimeSimulation.push_back(SimTime());
    }
    else{
      occupanyOfAp.push_back((double)100);
      TimeSimulation.push_back(SimTime());
    }

    if (trafficLoad >= 100){
      for (int i=0;i<(int)vectorOfConnectedStations.size();i++){
        Connection update;
        update.header.sourceID = apID;
        update.header.destinationID = vectorOfConnectedStations.at(i);
        update.Ap_Load = trafficLoad;
        (outAssignAirTime_f(update));
      }
    }
  }
}


#line 410 "ap.h"
void compcxx_AP_9 :: inUpdateConnection(StationInfo &info, int oldAP){

  if (apID == oldAP){
    for (int i=0; i<(int)vectorOfConnectedStations.size(); i++){
      if (vectorOfConnectedStations.at(i) == info.header.sourceID){
        vectorOfConnectedStations.erase(vectorOfConnectedStations.begin()+i);
        vectorOfConnectedStationsRSSIs.erase(vectorOfConnectedStationsRSSIs.begin()+i);
        numOfConnectedStations = numOfConnectedStations - 1;
      }
    }
  }

  if (apID == info.header.destinationID){
    numOfConnectedStations++;
    vectorOfConnectedStations.push_back(info.header.sourceID);
    vectorOfConnectedStationsRSSIs.push_back(info.RSSI);

    APBeacon beacon;
    beacon.header.destinationID = info.header.sourceID;
    beacon.header.sourceID = apID;
    beacon.header.X = X;
    beacon.header.Y = Y;
    beacon.header.Z = Z;
    beacon.Tx_Power = txPower;
    beacon.freq = frequency;
    beacon.protocolType = IEEEprotocolType;
    beacon.BW = channelBW;

    (outChannelChange_f(beacon));
  }
}


#line 442 "ap.h"
void compcxx_AP_9 :: inUpdateAttachedStationsParams (StationInfo &info){

  if (info.header.destinationID == apID){
    for (int i=0; i<(int)vectorOfConnectedStations.size(); i++){
      if (info.header.sourceID == vectorOfConnectedStations.at(i)){
        vectorOfConnectedStationsRSSIs.at(i) = info.RSSI;
      }
    }
  }
}


#line 453 "ap.h"
void compcxx_AP_9 :: CHselectionBylearning(trigger_t&){

  int lastAction = actionSelected;
  int num_arms = setOfactions.size();
  int index = SearchAction(lastAction, num_arms, &setOfactions);

  switch (APlearningFlag) {

    case 1:{

      if (flag == 0){
        int i = rand()%num_arms;
        actionSelected = setOfactions.at(i);
        TimesActionIsPicked[i] = TimesActionIsPicked[i] + 1;

        flag++;
      }
      else{
        double epsilon = 1/sqrt(iter);
        reward_action[index] = GetReward(lastAction, &channel_reward, &Action_Selected, &TimeSimulation, SimTime());
        occupancy_CH[index] = GetOccupancy(lastAction, &occupanyOfAp, &Action_Selected, &TimeSimulation, SimTime());

        actionSelected = setOfactions.at(Egreedy(num_arms, &reward_action, &occupancy_CH, epsilon, &TimesActionIsPicked));
      }

      for (int i=0; i<(int)CHMapToAction.size();i++){
        int overlap = ChannelOverlappingDetector(IEEEprotocolType,OperatingChannel,CHMapToAction.at(i));
        if (overlap ==1){
          trafficLoad = trafficLoad-CH_occupancy_detected.at(i);
          if (trafficLoad < 0.00001){
            trafficLoad = 0;
          }
        }
      }

      double* selectedConfiguration = GetConfiguration(IEEEprotocolType, actionSelected);
      channelBW = *selectedConfiguration;
      OperatingChannel = *(selectedConfiguration+1);
      frequency = *(selectedConfiguration+2);

      if ((int)vectorOfNeighboringAPs.size() != 0){
        for (int n=0;n<(int)vectorOfNeighboringAPs.size();n++){
          if(trafficLoad != 0){

            ApNotification RemoveLoad;
            RemoveLoad.Load = trafficLoad;
            RemoveLoad.ChannelNumber = CHMapToAction.at(index);
            RemoveLoad.flag = 0;
            RemoveLoad.header.destinationID = vectorOfNeighboringAPs.at(n);
            (outLoadToNeighbor_f(RemoveLoad));


            ApNotification AddLoad;
            AddLoad.Load = trafficLoad;
            AddLoad.ChannelNumber = OperatingChannel;
            AddLoad.flag = 1;
            AddLoad.header.destinationID = vectorOfNeighboringAPs.at(n);
            (outLoadToNeighbor_f(AddLoad));
          }
        }
      }

      for (int i=0; i<(int)CHMapToAction.size();i++){
        int overlap = ChannelOverlappingDetector(IEEEprotocolType,OperatingChannel,CHMapToAction.at(i));
        if (overlap ==1){
          trafficLoad = trafficLoad+CH_occupancy_detected.at(i);
        }
      }

      for (int i=0;i<(int)vectorOfConnectedStations.size();i++){
        APBeacon beacon;
        beacon.header.destinationID = vectorOfConnectedStations.at(i);
        beacon.header.sourceID = apID;
        beacon.header.X = X;
        beacon.header.Y = Y;
        beacon.header.Z = Z;
        beacon.Tx_Power = txPower;
        beacon.freq = frequency;
        beacon.protocolType = IEEEprotocolType;
        beacon.BW = channelBW;
        (outChannelChange_f(beacon));

        Connection update;
        update.header.sourceID = apID;
        update.header.destinationID = vectorOfConnectedStations.at(i);
        update.Ap_Load = trafficLoad;
        (outAssignAirTime_f(update));
      }

      ActionChange.push_back(actionSelected);
      TimeStamp.push_back(SimTime());

      trigger_Action.Set(SimTime()+ChSelectionTime);
      iter++;

    }break;

    case 2:{
      if (flag == 0){
        estimated_reward_action[index] = ((estimated_reward_action[index] * TimesActionIsPicked[index]) + (reward_action[index])) / (TimesActionIsPicked[index] + 2);
        estimated_reward_Per_action[index].push_back(estimated_reward_action[index]);
        estimated_reward_Per_action_Time[index].push_back(SimTime());
        actionSelected = setOfactions.at(ThompsonSampling(num_arms, &estimated_reward_action, &occupancy_CH, &TimesActionIsPicked));

        flag++;
      }
      else{
        reward_action[index] = GetReward(lastAction, &channel_reward, &Action_Selected, &TimeSimulation, SimTime());
        occupancy_CH[index] = GetOccupancy(lastAction, &occupanyOfAp, &Action_Selected, &TimeSimulation, SimTime());
        estimated_reward_action[index] = ((estimated_reward_action[index] * TimesActionIsPicked[index]) + (reward_action[index])) / (TimesActionIsPicked[index] + 2);
        estimated_reward_Per_action[index].push_back(estimated_reward_action[index]);
        estimated_reward_Per_action_Time[index].push_back(SimTime());
        actionSelected = setOfactions.at(ThompsonSampling(num_arms, &estimated_reward_action, &occupancy_CH, &TimesActionIsPicked));
      }

      for (int i=0; i<(int)CHMapToAction.size();i++){
        int overlap = ChannelOverlappingDetector(IEEEprotocolType,OperatingChannel,CHMapToAction.at(i));
        if (overlap ==1){
          trafficLoad = trafficLoad-CH_occupancy_detected.at(i);
          if (trafficLoad < 0.00001){
            trafficLoad = 0;
          }
        }
      }

      double* selectedConfiguration = GetConfiguration(IEEEprotocolType, actionSelected);
      channelBW = *selectedConfiguration;
      OperatingChannel = *(selectedConfiguration+1);
      frequency = *(selectedConfiguration+2);

      if ((int)vectorOfNeighboringAPs.size() != 0){
        for (int n=0;n<(int)vectorOfNeighboringAPs.size();n++){
          if(trafficLoad != 0){

            ApNotification RemoveLoad;
            RemoveLoad.Load = trafficLoad;
            RemoveLoad.ChannelNumber = CHMapToAction.at(index);
            RemoveLoad.flag = 0;
            RemoveLoad.header.destinationID = vectorOfNeighboringAPs.at(n);
            (outLoadToNeighbor_f(RemoveLoad));


            ApNotification AddLoad;
            AddLoad.Load = trafficLoad;
            AddLoad.ChannelNumber = OperatingChannel;
            AddLoad.flag = 1;
            AddLoad.header.destinationID = vectorOfNeighboringAPs.at(n);
            (outLoadToNeighbor_f(AddLoad));
          }
        }
      }

      for (int i=0; i<(int)CHMapToAction.size();i++){
        int overlap = ChannelOverlappingDetector(IEEEprotocolType,OperatingChannel,CHMapToAction.at(i));
        if (overlap ==1){
          trafficLoad = trafficLoad+CH_occupancy_detected.at(i);
        }
      }

      for (int i=0;i<(int)vectorOfConnectedStations.size();i++){
        APBeacon beacon;
        beacon.header.destinationID = vectorOfConnectedStations.at(i);
        beacon.header.sourceID = apID;
        beacon.header.X = X;
        beacon.header.Y = Y;
        beacon.header.Z = Z;
        beacon.Tx_Power = txPower;
        beacon.freq = frequency;
        beacon.protocolType = IEEEprotocolType;
        beacon.BW = channelBW;
        (outChannelChange_f(beacon));

        Connection update;
        update.header.sourceID = apID;
        update.header.destinationID = vectorOfConnectedStations.at(i);
        update.Ap_Load = trafficLoad;
        (outAssignAirTime_f(update));
      }

      ActionChange.push_back(actionSelected);
      TimeStamp.push_back(SimTime());

      trigger_Action.Set(SimTime()+ChSelectionTime);
    }break;
  }
}


#line 640 "ap.h"
void compcxx_AP_9 :: ProgressFunct(trigger_t&){
  printf("Progress: %f\n", (SimTime()/(double)86400)*100);
  trigger_progress.Set(SimTime()+4320);
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
  (p_compcxx_parent->APselectionBylearning(m_event.data));
}




#line 288 "../COST/cost.h"

#line 288 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_5/*<trigger_t >*/::Set(trigger_t const & data, double time)
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
*/void compcxx_Timer_5/*<trigger_t >*/::Set(double time)
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
*/void compcxx_Timer_5/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 319 "../COST/cost.h"

#line 319 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_5/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->ProcessBeacons(m_event.data));
}




#line 288 "../COST/cost.h"

#line 288 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_6/*<trigger_t >*/::Set(trigger_t const & data, double time)
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
*/void compcxx_Timer_6/*<trigger_t >*/::Set(double time)
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
*/void compcxx_Timer_6/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 319 "../COST/cost.h"

#line 319 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_6/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->SendRequestedAT(m_event.data));
}




#line 288 "../COST/cost.h"

#line 288 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_7/*<trigger_t >*/::Set(trigger_t const & data, double time)
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
*/void compcxx_Timer_7/*<trigger_t >*/::Set(double time)
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
*/void compcxx_Timer_7/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 319 "../COST/cost.h"

#line 319 "../COST/cost.h"
/*template <class T>
*/void compcxx_Timer_7/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->SendTxTimeFinished(m_event.data));
}




#line 94 "station.h"

#line 95 "station.h"

#line 96 "station.h"

#line 97 "station.h"

#line 101 "station.h"
compcxx_Station_10 :: compcxx_Station_10(){
  trigger_ProcessBeacons.p_compcxx_parent=this /*connect trigger_ProcessBeacons.to_component,*/;
  trigger_SendRequestedAT.p_compcxx_parent=this /*connect trigger_SendRequestedAT.to_component,*/;
  trigger_TxTimeFinished.p_compcxx_parent=this /*connect trigger_TxTimeFinished.to_component,*/;
  trigger_Action.p_compcxx_parent=this /*connect trigger_Action.to_component,*/;
}


#line 108 "station.h"
void compcxx_Station_10 :: Setup(){

}


#line 112 "station.h"
void compcxx_Station_10 :: Start(){
  iter = 1;
  flag = 0;
  TimeSizeF = 0;
  TimeSizeS = 0;
}


#line 119 "station.h"
void compcxx_Station_10 :: Stop(){

  int TimeWrap = 900;
  double pos = runTimeSim/TimeWrap;
  int step = 0;
  int count = 0;
  int endVal = TimeWrap-1;
  double tmp_mean = 0;
  double m = 0;

  for (int k=0; k<pos; k++){
    for (int init=0; init<(int)timeSim2.size(); init++){
      if (step <= timeSim2.at(init) && timeSim2.at(init) < endVal){
        tmp_mean = tmp_mean + Satisfaction.at(init);
        count = count + 1;
      }
    }
    m = tmp_mean/count;

    mSat.push_back(m);

    tmp_mean = 0;
    m = 0;
    count = 0;
    step = step+TimeWrap;
    endVal = endVal+TimeWrap;
  }
}


#line 148 "station.h"
void compcxx_Station_10 :: inReceivedBeacon(APBeacon &b){

  detectedWLANs.push_back(b);
  trigger_ProcessBeacons.Set(SimTime()+0.001);
}


#line 154 "station.h"
void compcxx_Station_10 :: ProcessBeacons(trigger_t&){

  StationInfo info;

  std::vector<double>RSSIvalueUL;
  std::vector<double>DLDataRates;
  std::vector<double>ULDataRates;
  std::vector<double>InRangeAPsRSSI;

  if (detectedWLANs.size() !=0){

    while (detectedWLANs.size() > 0) {
      double tmpRSSIvalueDL, tmpRSSIvalueUL, DL_r, UL_r;

      APBeacon b = detectedWLANs.front();
      tmpRSSIvalueUL = txPower - PropL(X, Y, Z, b.header.X, b.header.Y, b.header.Z, b.freq);

      if (tmpRSSIvalueUL> -80){

        tmpRSSIvalueDL = b.Tx_Power - PropL(X, Y, Z, b.header.X, b.header.Y, b.header.Z, b.freq);

        InRangeAPs.push_back(b.header.sourceID);
        InRangeAPsRSSI.push_back(tmpRSSIvalueDL);
        RSSIvalueUL.push_back(tmpRSSIvalueUL);

        DL_r = SetDataRate(tmpRSSIvalueDL, b.protocolType, b.BW);
        UL_r = SetDataRate(tmpRSSIvalueUL, b.protocolType, b.BW);

        DLDataRates.push_back(DL_r);
        ULDataRates.push_back(UL_r);

        if (tmpRSSIvalueUL >= -75)
        {
          CandidateAPs.push_back(b.header.sourceID);
        }
      }
      detectedWLANs.pop_front();
    }
  }

  switch (stationLearningFlag) {
    case 0:{ 
        double RSSI_UL;

        RSSI = -100;
        servingAP = 0;

        for (int i=0; i<(int)InRangeAPsRSSI.size(); i++){
          if (RSSI<=InRangeAPsRSSI.at(i)){
            servingAP = InRangeAPs.at(i);
            RSSI = InRangeAPsRSSI.at(i);
            RSSI_UL = RSSIvalueUL.at(i);
            DLDataRate = DLDataRates.at(i);
            ULDataRate = ULDataRates.at(i);
          }
        }

        info.header.sourceID = staID;
        info.header.destinationID = servingAP;
        info.RSSI = RSSI_UL;

        (outSetClientAssociation_f(info));
        trigger_SendRequestedAT.Set(SimTime()+Exponential(flowActivation));

      }break;

    case 1:{ 

        double RSSI_UL;
        int size = (int)CandidateAPs.size();

        RSSI = -100;
        servingAP = 0;

        reward_action.assign(size,0.0);
        occupancy_AP.assign(size,0.0);
        Times_ActionSelected.assign(size,0.0);

        for (int i=0; i<(int)InRangeAPsRSSI.size(); i++){
          if (RSSI<=InRangeAPsRSSI.at(i)){
            servingAP = InRangeAPs.at(i);
            RSSI = InRangeAPsRSSI.at(i);
            RSSI_UL = RSSIvalueUL.at(i);
            DLDataRate = DLDataRates.at(i);
            ULDataRate = ULDataRates.at(i);
          }
        }

        info.header.sourceID = staID;
        info.header.destinationID = servingAP;
        info.RSSI = RSSI_UL;
        (outSetClientAssociation_f(info));

        if (size > 1){
          trigger_Action.Set(SimTime()+offsetSTA);
        }
        trigger_SendRequestedAT.Set(SimTime()+Exponential(flowActivation));

    }break;

    case 2:{ 

        double RSSI_UL;
        int size = (int)CandidateAPs.size();

        RSSI = -100;
        servingAP = 0;

        reward_action.assign(size,0.0);
        occupancy_AP.assign(size,0.0);
        estimated_reward_action.assign(size,0.0);
        Times_ActionSelected.assign(size,0.0);
        estimated_reward_Per_action.resize(size);
        estimated_reward_Per_action_Time.resize(size);

        for (int i=0; i<(int)InRangeAPsRSSI.size(); i++){
          if (RSSI<=InRangeAPsRSSI.at(i)){
            servingAP = InRangeAPs.at(i);
            RSSI = InRangeAPsRSSI.at(i);
            RSSI_UL = RSSIvalueUL.at(i);
            DLDataRate = DLDataRates.at(i);
            ULDataRate = ULDataRates.at(i);
          }
        }

        info.header.sourceID = staID;
        info.header.destinationID = servingAP;
        info.RSSI = RSSI_UL;
        (outSetClientAssociation_f(info));

        if ((size > 1)&&(userType == 2)){
          trigger_Action.Set(SimTime()+offsetSTA);
        }

        trigger_SendRequestedAT.Set(SimTime()+Exponential(flowActivation));

    }break;
  }

  ActionChange.push_back(servingAP);
  TimeStamp.push_back(SimTime());

}


#line 298 "station.h"
void compcxx_Station_10 :: SendRequestedAT(trigger_t&){

  double TimeMPDU, Tack, Trts, Tcts, LDBPS_DL, LDBPS_UL;

  Connection ConnRequest;

  LDBPS_DL = (DLDataRate*pow(10,6))*Tofdm;
  LDBPS_UL = (ULDataRate*pow(10,6))*Tofdm;

  FlowType = 0; 

  requestedBW = Random(medBW) + 1;

  switch (FlowType) {
    case 0:{ 

      TimeMPDU = TphyHE + std::ceil(((Lsf+Lmac+frameLength+Ltb)/(LDBPS_DL)))*Tofdm;
      Tack = TphyL + std::ceil(((Lsf+Lack+Ltb)/(legacyRate)))*Tofdm_leg;
      Trts = TphyL + std::ceil(((Lsf+Lrts+Ltb)/(legacyRate)))*Tofdm_leg;
      Tcts = TphyL + std::ceil(((Lsf+Lcts+Ltb)/(legacyRate)))*Tofdm_leg;

      AirTimeRequired = (std::ceil((requestedBW*pow(10,6)/frameLength))*(1/(1-Pe)))*(((CW/2)*Tempty)+(Trts+Tsifs+Tcts+Tsifs+TimeMPDU+Tsifs+Tack+Tdifs+Tempty));

      ConnRequest.header.sourceID = staID;
      ConnRequest.header.destinationID = servingAP;
      ConnRequest.LoadByStation = AirTimeRequired*100;
      break;
    }
    case 1:{ 
      TimeMPDU = TphyHE + std::ceil(((Lsf+Lmac+frameLength+Ltb)/(LDBPS_UL)))*Tofdm;
      Tack = TphyL + std::ceil(((Lsf+Lack+Ltb)/(legacyRate)))*Tofdm_leg;
      Trts = TphyL + std::ceil(((Lsf+Lrts+Ltb)/(legacyRate)))*Tofdm_leg;
      Tcts = TphyL + std::ceil(((Lsf+Lcts+Ltb)/(legacyRate)))*Tofdm_leg;

      AirTimeRequired = (std::ceil((requestedBW*pow(10,6)/frameLength))*(1/(1-Pe)))*(((CW/2)*Tempty)+(Trts+Tsifs+Tcts+Tsifs+TimeMPDU+Tsifs+Tack+Tdifs+Tempty));

      ConnRequest.header.sourceID = staID;
      ConnRequest.header.destinationID = servingAP;
      ConnRequest.LoadByStation = AirTimeRequired*100;
      break;
    }
  }

  startTX = SimTime();
  TimeSizeS = (int)timeSim2.size();

  (outRequestAirTime_f(ConnRequest));

  AirTimeEvo.push_back(AirTimeRequired);
  timeSim.push_back(SimTime());

  trigger_TxTimeFinished.Set(SimTime()+Exponential(flowDuration));
}


#line 352 "station.h"
void compcxx_Station_10 :: inAssignedAirTime(Connection &response){

  if (staID == response.header.destinationID){
    double MaxLoad;

    if (0 < AirTimeRequired){
      MaxLoad = 100;
      if (response.Ap_Load < 100){
        CandidateAPsTrafficLoad.push_back(response.Ap_Load);
        Satisfaction.push_back((double)1);
      }
      else{
        CandidateAPsTrafficLoad.push_back((double)100);
        Satisfaction.push_back(((std::min(MaxLoad,response.Ap_Load))/(response.Ap_Load)));
      }
      Action_Selected.push_back(servingAP);
      timeSim2.push_back(SimTime());
    }
  }
}


#line 373 "station.h"
void compcxx_Station_10 :: SendTxTimeFinished(trigger_t&){

  finishTX = SimTime();
  TimeSizeF = (int)timeSim2.size();
  int size = TimeSizeF-TimeSizeS;
  double throughput = GetData(FlowType, TimeSizeS, size, &Satisfaction, requestedBW);

  Throughput.push_back(throughput);
  bits_Sent = bits_Sent + (GetData(FlowType, TimeSizeS, size, &Satisfaction, requestedBW)*(finishTX-startTX));
  totalBits = totalBits + (requestedBW*(finishTX-startTX));
  

  Connection ConnFinish;
  ConnFinish.header.destinationID = servingAP;
  ConnFinish.header.sourceID = staID;
  ConnFinish.LoadByStation = AirTimeRequired*100;

  (outFlowEnded_f(ConnFinish));

  AirTimeRequired = 0;
  AirTimeEvo.push_back(AirTimeRequired);
  timeSim.push_back(SimTime());

  trigger_SendRequestedAT.Set(SimTime()+Exponential(flowActivation));
}


#line 399 "station.h"
void compcxx_Station_10 :: inUpdateStationParameters(APBeacon &b){

  if (staID == b.header.destinationID){
    double RSSIvalueUL;
    StationInfo info;

    RSSI = b.Tx_Power - PropL(X,Y,Z,b.header.X,b.header.Y,b.header.Z,b.freq);
    RSSIvalueUL = txPower - PropL(X,Y,Z,b.header.X,b.header.Y,b.header.Z,b.freq);
    frequency = b.freq;
    IEEE_protocol = b.protocolType;
    ChBW = b.BW;

    DLDataRate = SetDataRate(RSSI, IEEE_protocol, ChBW);
    ULDataRate = SetDataRate(RSSIvalueUL, IEEE_protocol, ChBW);

    info.header.sourceID = staID;
    info.header.destinationID = b.header.sourceID;
    info.RSSI = RSSI;

    (outUpdateAttachedStationsParams_f(info));
  }
}


#line 422 "station.h"
void compcxx_Station_10 :: APselectionBylearning(trigger_t&){

  if (trigger_TxTimeFinished.Active() == 0){

    int oldAP = servingAP;
    int num_arms = CandidateAPs.size();
    int index = SearchAction(servingAP, num_arms, &CandidateAPs);

    switch (stationLearningFlag) {

      case 1:{  

        if (flag == 0){
          int i = rand()%num_arms;
          servingAP = CandidateAPs.at(i);
          Times_ActionSelected[i] = Times_ActionSelected[i] + 1;

          flag++;
        }
        else{
          double epsilon = 1/sqrt(iter);
          reward_action[index] = GetReward(servingAP, &Satisfaction, &Action_Selected, &timeSim2, SimTime());
          occupancy_AP[index] = GetOccupancy(servingAP, &CandidateAPsTrafficLoad, &Action_Selected, &timeSim2, SimTime());
          servingAP = CandidateAPs.at(Egreedy(num_arms, &reward_action, &occupancy_AP, epsilon, &Times_ActionSelected));
        }

        StationInfo hello;
        hello.header.sourceID = staID;
        hello.header.destinationID = servingAP;
        hello.RSSI = 0;

        if (oldAP != servingAP){
          (outUpdateConnection_f(hello, oldAP));
        }

        ActionChange.push_back(servingAP);
        TimeStamp.push_back(SimTime());

        trigger_Action.Set(SimTime()+SearchBestAP);
        iter++;

      }break;

      case 2:{  

        if (flag == 0){
          estimated_reward_action[index] = ((estimated_reward_action[index] * Times_ActionSelected[index]) + (reward_action[index]))/ (Times_ActionSelected[index] + 2);
          estimated_reward_Per_action[index].push_back(estimated_reward_action[index]);
          estimated_reward_Per_action_Time[index].push_back(SimTime());
          servingAP = CandidateAPs.at(ThompsonSampling(num_arms, &estimated_reward_action, &occupancy_AP, &Times_ActionSelected));
          flag++;
        }
        else{
          reward_action[index] = GetReward(servingAP, &Satisfaction, &Action_Selected, &timeSim2, SimTime());
          occupancy_AP[index] = (GetOccupancy(servingAP, &CandidateAPsTrafficLoad, &Action_Selected, &timeSim2, SimTime()))/100;
          estimated_reward_action[index] = ((estimated_reward_action[index] * Times_ActionSelected[index]) + (reward_action[index]))/ (Times_ActionSelected[index] + 2);
          estimated_reward_Per_action[index].push_back(estimated_reward_action[index]);
          estimated_reward_Per_action_Time[index].push_back(SimTime());
          servingAP = CandidateAPs.at(ThompsonSampling(num_arms, &estimated_reward_action, &occupancy_AP, &Times_ActionSelected));
        }

        StationInfo hello;
        hello.header.sourceID = staID;
        hello.header.destinationID = servingAP;
        hello.RSSI = 0;

        if (oldAP != servingAP){
          (outUpdateConnection_f(hello, oldAP));
        }

        ActionChange.push_back(servingAP);
        TimeStamp.push_back(SimTime());

        trigger_Action.Set(SimTime()+SearchBestAP);
      }break;
    }
  }

  else{
    double t = trigger_TxTimeFinished.GetTime() - SimTime() + 0.001;
    trigger_Action.Cancel();
    trigger_Action.Set(SimTime()+t);
  }
}


#line 45 "neko.cc"
void compcxx_Neko_11 :: Setup(){

  if (rnd == 1){
    GenerateRandom();
  }
  else{
    LoadScenario();
  }

  for (int j=0; j<numOfStations; j++){
    for (int i=0; i<numOfAPs; i++){

        STA_container[j].outSetClientAssociation_f.Connect(APoint_container[i],(compcxx_component::Station_outSetClientAssociation_f_t)&compcxx_AP_9::inSetClientAssociation) /*connect STA_container[j].outSetClientAssociation,APoint_container[i].inSetClientAssociation*/;
        STA_container[j].outRequestAirTime_f.Connect(APoint_container[i],(compcxx_component::Station_outRequestAirTime_f_t)&compcxx_AP_9::inRequestedAirTime) /*connect STA_container[j].outRequestAirTime,APoint_container[i].inRequestedAirTime*/;
        STA_container[j].outFlowEnded_f.Connect(APoint_container[i],(compcxx_component::Station_outFlowEnded_f_t)&compcxx_AP_9::inTxTimeFinished) /*connect STA_container[j].outFlowEnded,APoint_container[i].inTxTimeFinished*/;
        STA_container[j].outUpdateConnection_f.Connect(APoint_container[i],(compcxx_component::Station_outUpdateConnection_f_t)&compcxx_AP_9::inUpdateConnection) /*connect STA_container[j].outUpdateConnection,APoint_container[i].inUpdateConnection*/;
        STA_container[j].outUpdateAttachedStationsParams_f.Connect(APoint_container[i],(compcxx_component::Station_outUpdateAttachedStationsParams_f_t)&compcxx_AP_9::inUpdateAttachedStationsParams) /*connect STA_container[j].outUpdateAttachedStationsParams,APoint_container[i].inUpdateAttachedStationsParams*/;

        APoint_container[i].outSendBeaconToNodes_f.Connect(STA_container[j],(compcxx_component::AP_outSendBeaconToNodes_f_t)&compcxx_Station_10::inReceivedBeacon) /*connect APoint_container[i].outSendBeaconToNodes,STA_container[j].inReceivedBeacon*/;
        APoint_container[i].outAssignAirTime_f.Connect(STA_container[j],(compcxx_component::AP_outAssignAirTime_f_t)&compcxx_Station_10::inAssignedAirTime) /*connect APoint_container[i].outAssignAirTime,STA_container[j].inAssignedAirTime*/;
        APoint_container[i].outChannelChange_f.Connect(STA_container[j],(compcxx_component::AP_outChannelChange_f_t)&compcxx_Station_10::inUpdateStationParameters) /*connect APoint_container[i].outChannelChange,STA_container[j].inUpdateStationParameters*/;
    }
  }

  for (int i=0;i<numOfAPs;i++){
    for (int j=0; j<numOfAPs; j++){
      if (APoint_container[i].apID != APoint_container[j].apID){
        APoint_container[i].outLoadToNeighbor_f.Connect(APoint_container[j],(compcxx_component::AP_outLoadToNeighbor_f_t)&compcxx_AP_9::inLoadFromNeighbor) /*connect APoint_container[i].outLoadToNeighbor,APoint_container[j].inLoadFromNeighbor*/;
        APoint_container[i].outSetNeighbors_f.Connect(APoint_container[j],(compcxx_component::AP_outSetNeighbors_f_t)&compcxx_AP_9::inSetNeighbors) /*connect APoint_container[i].outSetNeighbors,APoint_container[j].inSetNeighbors*/;
      }
    }
  }
}


#line 79 "neko.cc"
void compcxx_Neko_11 :: Start(){

  
}


#line 84 "neko.cc"
void compcxx_Neko_11 :: Stop(){

  FILE* pFileSTATS = fopen("../Output/STATS.txt","a");
  float tmpSat, tmpTh, tmpDRatio, simSat, simTh, simDRatio;
  std::vector<double> mean_Satisfaction;
  double m_val = 0;

  for (int i=0; i<(int)STA_container[0].mSat.size(); i++){
    for (int j=0; j<numOfStations; j++){
      m_val += STA_container[j].mSat.at(i);
    }

    mean_Satisfaction.push_back(m_val/numOfStations);
    m_val = 0;
  }

  simTh = 0;
  simDRatio = 0;
  simSat = 0;

  tmpSat = 0;
  tmpTh = 0;
  tmpDRatio = 0;

  for (int i=0; i<(int)mean_Satisfaction.size(); i++){
    tmpSat += mean_Satisfaction.at(i);
  }

  simSat = tmpSat/(int)mean_Satisfaction.size();

  for (int i=0; i<numOfStations; i++){
    tmpTh += std::accumulate(STA_container[i].Throughput.begin(),STA_container[i].Throughput.end(), 0.0)/(int)STA_container[i].Throughput.size();
    tmpDRatio += 1-(STA_container[i].bits_Sent/STA_container[i].totalBits);
  }

  simTh = tmpTh;
  simDRatio = tmpDRatio/numOfStations;
  printf("Th: %f\n", simTh);
  fprintf(pFileSTATS, "%i; %f; %f; %f\n", seed, simSat, simTh, (simDRatio*100));

  fclose(pFileSTATS);
}


#line 127 "neko.cc"
void compcxx_Neko_11 :: GenerateRandom(){

  int Xaxis = 30;
  int Yaxis = 30;
  int Zaxis = 2;
  int index = 0;
  int stationsPerAP = 15;

  numOfAPs = 20;
  numOfStations = numOfAPs*stationsPerAP;

  APoint_container.SetSize(numOfAPs);
  STA_container.SetSize(numOfStations);

  int user = 0;

  for (int i=0; i<numOfAPs; i++){
    APoint_container[i].apID = i;
    APoint_container[i].X = ((double)rand() / RAND_MAX) * Xaxis;
    APoint_container[i].Y = ((double)rand() / RAND_MAX) * Yaxis;
    APoint_container[i].Z = ((double)rand() / RAND_MAX) * Zaxis;
    APoint_container[i].txPower = 15;
    APoint_container[i].CCA = -80;
    APoint_container[i].actionSelected = rand()%3;
    APoint_container[i].IEEEprotocolType = 2;
    APoint_container[i].trafficLoad = 0;


    for (int j=0; j<stationsPerAP; j++){
      int k = 0;
      while (k<1){
        double tmpX = ((double)rand() / RAND_MAX) * Xaxis;
        double tmpY = ((double)rand() / RAND_MAX) * Yaxis;
        double tmpZ = ((double)rand() / RAND_MAX) * Zaxis;

        float propL = PropL(tmpX,tmpY,tmpZ,APoint_container[i].X,APoint_container[i].Y,APoint_container[i].Z, 5.32);
        float RSSI = 15-propL;

        if ((-80 <= RSSI) && (RSSI < -45)){
          STA_container[index].staID = index;
          STA_container[index].X = tmpX;
          STA_container[index].Y = tmpY;
          STA_container[index].Z = tmpZ;
          STA_container[index].txPower = 15;

          double rndProbability = ((double) rand() / (RAND_MAX));

          if (rndProbability <= ProbUserAgentEnabled){
            STA_container[index].userType = 2;
            user++;
          }
          else{
            STA_container[index].userType = 1;
          }
          
          k++;
          index++;
        }
      }
    }
  }
  printf("user: %i\n", user);
}


#line 191 "neko.cc"
void compcxx_Neko_11 :: LoadScenario(){

  FILE* inputFileName = fopen("../Input/Inputfile.txt", "r");
  char line [100];
  char *str;
  char ap[] = "AP";
  char sta[] = "STA";
  int i = 0;
  int j = 0;

  numOfAPs = 0;
  numOfStations = 0;

  fgets(line,100,inputFileName); 

  while ((fgets(line,100,inputFileName))){
    str = strtok(line, ";");
    if (strcmp(ap, str) == 0){
      numOfAPs++;
    }
    else if(strcmp(sta, str) == 0){
      numOfStations++;
    }
  }

  APoint_container.SetSize(numOfAPs);
  STA_container.SetSize(numOfStations);

  rewind (inputFileName);
  fgets(line,100,inputFileName); 

  while ((fgets(line,100,inputFileName))){
    str = strtok(line, ";");

    if (strcmp(ap, str) == 0){
      APoint_container[i].apID = i;
      APoint_container[i].X = atof(strtok(NULL, ";"));
      APoint_container[i].Y = atof(strtok(NULL, ";"));
      APoint_container[i].Z = atof(strtok(NULL, ";"));
      APoint_container[i].txPower = atoi(strtok(NULL, ";"));
      APoint_container[i].CCA = atof(strtok(NULL, ";"));
      APoint_container[i].actionSelected = atoi(strtok(NULL, ";"));
      APoint_container[i].IEEEprotocolType = atoi(strtok(NULL, ";"));
      APoint_container[i].trafficLoad = atof(strtok(NULL, ";\n"));
      i++;
    }
    else if(strcmp(sta, str) == 0){
      STA_container[j].staID = j;
      STA_container[j].X = atof(strtok(NULL, ";"));
      STA_container[j].Y = atof(strtok(NULL, ";"));
      STA_container[j].Z = atof(strtok(NULL, ";"));
      STA_container[j].txPower = atoi(strtok(NULL, ";"));
      STA_container[j].userType = atoi(strtok(NULL, ";"));
      j++;
    }
  }
  fclose(inputFileName);
}


#line 250 "neko.cc"
int main(int argc, char *argv[]){

  compcxx_Neko_11 test;

  if (rnd == 1) {
    test.seed = atoi(argv[1]);
    test.Seed = test.seed;
    srand(test.seed);
  }
  else{
    srand(time(NULL));
    test.Seed=rand()%100;
  }

  test.StopTime(runTimeSim);
  test.Setup();
  test.Run();
}
