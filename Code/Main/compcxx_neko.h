#include <assert.h> 
 #include<vector> 
template <class T> class compcxx_array { public: 
virtual ~compcxx_array() { for (typename std::vector<T*>::iterator i=m_elements.begin();i!=m_elements.end();i++) delete (*i); } 
void SetSize(unsigned int n) { for(unsigned int i=0;i<n;i++)m_elements.push_back(new T); } 
T& operator [] (unsigned int i) { assert(i<m_elements.size()); return(*m_elements[i]); } 
unsigned int size() { return m_elements.size();} 
private: std::vector<T*> m_elements; }; 
class compcxx_component; 
template <class T> class compcxx_functor {public: 
void Connect(compcxx_component&_c, T _f){ c.push_back(&_c); f.push_back(_f); } 
protected: std::vector<compcxx_component*>c; std::vector<T> f; }; 
class compcxx_component { public: 
typedef void  (compcxx_component::*AP_outDataSTA_f_t)(Flow &q);
typedef void  (compcxx_component::*AP_outCtrlApp_f_t)(AppCTRL &n);
typedef void  (compcxx_component::*AP_outCtrlSTA_f_t)(Notification &n);
typedef void  (compcxx_component::*AP_outCtrlAP_f_t)(Notification &n);
typedef void  (compcxx_component::*Application_outCtrlAP_f_t)(AppCTRL &n);
typedef void  (compcxx_component::*STA_outCtrlAP_f_t)(Notification &n);
};
