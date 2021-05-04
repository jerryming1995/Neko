
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
