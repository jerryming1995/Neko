// %%%%%%%%%% -- General packets -- %%%%%%%%%% //

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

  /* For further development:

  int SupportedDataRates;
  int SpatialStreams; (In case of SS or MIMO)

  */
};

// %%%%%%%%%% -- AP-AP messages -- %%%%%%%%%% //

struct ApNotification{

  Header header;

  int ChannelNumber;
  double Load;
  int flag;
};

// %%%%%%%%%% -- Station-AP -- %%%%%%%%%% //

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
