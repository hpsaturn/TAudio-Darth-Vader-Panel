class mOTACallbacks : public OTAHandlerCallbacks {
  void onStart() {
    cancelShutdown();
  }
  void onProgress(unsigned int progress, unsigned int total) {}
  void onEnd() {}
  void onError() {}
  void onProgress(float progress) {}
};