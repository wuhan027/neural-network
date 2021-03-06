#ifndef ANN_ACTIVATION_FUNCTION_H
#define ANN_ACTIVATION_FUNCTION_H
#include <Debug.h>
#include <cmath>

namespace ANN {
  // Activation functions designed in the form of CRTP.
  template<typename WeightType>
  class Activation {
    public:
      // @TODO: Try this: T operator()(T t) {
      // FAQ: Even if I do not write this function as
      // template of WeightType and put T instead of WeightType, it works.
      // I don't quite understand why?
      //
      virtual WeightType Act(WeightType t) const = 0;
      // Derivative of linear activation function w.r.t. weight.
      virtual WeightType Deriv(WeightType t) const = 0;
  };

  template<typename WeightType>
  class LinearAct : public Activation<WeightType> {
    public:
      WeightType Act(WeightType w) const {
        return w;
      }
      // Derivative of linear activation function w.r.t. weight is 1
      WeightType Deriv(WeightType t) const {
        return WeightType(1);
      }
  };

  template<typename WeightType>
  class SigmoidAct : public Activation<WeightType> {
    public:
      WeightType Act(WeightType w) const {
        DEBUG2(dbgs() << "\nSigmoid function Input: " << w
                      << ", Output:" << std::tanh(w));
        return std::tanh(w);
      }
      // Derivative of tanh activation function w.r.t. weight is 1 - tanh(w)^2
      WeightType Deriv(WeightType t) const {
        auto tmp = std::tanh(t);
        return 1 - tmp*tmp;
      }
  };
} // namespace ANN



#endif // ANN_ACTIVATION_FUNCTION_H

