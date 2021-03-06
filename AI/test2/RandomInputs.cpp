#include "TrainingAlgorithms.h"

#include <TypeConversions.h>
#include <iostream>

using namespace ANN;
using namespace utilities;
// Sets up the single layer feed forward network
// and evaluates the output for a set of sample inputs.
int main() {
  const int ip_size = 3;
  Activation<NeuronWeightType>* act = new SigmoidAct<NeuronWeightType>;
  // Create a single layer feed forward neural network.
  auto NN = CreateSLFFN(ip_size, act);

  // Choose the training algorithm.
  float alpha = 0.01;
  ConvergenceMethod* CM = new SimpleDelta;
  Trainer T(NN, CM, alpha);

  // Validation of the output.
  typedef ValidateOutput<std::vector<bool>, bool> ValidatorType;
  ValidatorType Validator(NN);
  BoolAnd CostFunction;
  unsigned times_trained = 0;
  train:
  T.SetAlpha(alpha);
  DEBUG0(dbgs() << "\nTraining with alpha:" << alpha);
  for (unsigned i = 0; i < 10;) {
    std::vector<bool> RS = GetRandomizedSet(BooleanSampleSpace, ip_size-1);
    std::vector<float> RSF = BoolsToFloats(RS);
    // The last input is the bias.
    RSF.insert(RSF.begin(), -1);
    DEBUG0(dbgs() << "\nSample Inputs:"; PrintElements(dbgs(), RSF));
    //NN.PrintNNDigraph(*NN.GetRoot(), std::cout);
    auto op = NN.GetOutput(RSF);
    auto bool_op = FloatToBool(op);
    auto desired_op = Validator.GetDesiredOutput(&CostFunction, RS);
    // Is the output same as desired output?
    if (!Validator.Validate(&CostFunction, RS, bool_op)) {
      DEBUG0(dbgs() << "\tLearning (" << op
                    << ", " << bool_op
                    << ", " << desired_op << ")");
      //NN.PrintNNDigraph(*NN.GetRoot(), std::cout);
      // No => Train
      T.TrainNetworkFeedForward(RSF, desired_op);
      ++times_trained;
      i = 0;
      //NN.PrintNNDigraph(*NN.GetRoot(), std::cout);
    } else {
      ++i; // Increment trained counter.
      DEBUG0(dbgs() << "\tTrained (" << op << ", " << bool_op << ")");
    }
  }
  // Once the training has been done, test the network with
  // a large set of inputs in the sample space, possibly
  // covering the complete sample space.
  DEBUG0(dbgs() << "\nPrinting after training");
  for (unsigned i = 0; i < 20; ++i) {
    std::vector<bool> RS = GetRandomizedSet(BooleanSampleSpace, ip_size-1);
    std::vector<float> RSF = BoolsToFloats(RS);
    // The last input is the bias.
    RSF.insert(RSF.begin(), -1);
    auto op = NN.GetOutput(RSF);
    auto bool_op = FloatToBool(op);
    DEBUG0(dbgs() << "\nSample Inputs:"; PrintElements(dbgs(), RSF));
    if (Validator.Validate(&CostFunction, RS, bool_op))
      DEBUG0(dbgs() << "\tTrained: " << op << ", " << bool_op << ")");
    else {
      alpha = alpha < 0.4 ? 2*alpha : alpha;
      DEBUG0(dbgs() << "\tUnTrained: " << op << ", " << bool_op << ")");
      goto train;
    }
  }
  DEBUG0(dbgs() << "\nTrained for " << times_trained << " cycles.");
  NN.PrintNNDigraph(*NN.GetRoot(), std::cout);
  delete CM;
  return 0;
}
