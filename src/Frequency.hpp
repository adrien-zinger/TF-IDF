#include <utility>

class Frequency {
  public:
  /**
   * Get the frequency of a given word in all the directory
   * Return a frenquency, the number of time it appear, and the total number of file
   * 
   */
  std::pair<float, float> Find(std::string directory, std::string word);
};
