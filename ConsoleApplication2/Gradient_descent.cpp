#include <iostream>
#include <vector>
#include <chrono>
#include <random>        //random number generation
#include <algorithm>
#include <iterator>
#include <string>
#include <cmath>
#include <functional>
#include <math.h>       // for pi
#include <numeric>      // std::iota
#include <limits>       // for infinity function

#include <fstream>


using namespace std;

//random number generator
std::mt19937 rng(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

std::normal_distribution<double> generator(-1.0, 1.0);


//Calculate the vector norm of a vector
double getVectorNorm(vector<double> argVector) {
  double norm = 0.0;
  for (int i = 0; i < argVector.size(); ++i) {
    norm += argVector[i] * argVector[i];
  }
  norm = sqrt(norm);
  return norm;
}

class network
{
public:
  network(int dims);


  //Calculate sigma
  double calc_sigma(vector<double> input) {
    double sigma = 0;

    double w1i = 0; double w2i = 0;
    for (int i = 0; i < input.size(); ++i) {
      
      w1i += input[i] * w1[i];
      w2i += input[i] * w2[i];
    }

    sigma = tanh(w1i) + tanh(w2i);

    return sigma;
  }

  //Calculate gradient
  vector<double> calc_gradient(vector<double> input, double target, bool whichweight) {
    vector<double> gradient(input.size());
    double vec_prod = 0.0;
    vector<double> w;
    if (whichweight)
      w = w1;
    else
      w = w2;

    for (int i = 0; i < input.size(); ++i) {
      vec_prod += input[i] * w[i];
    }

    for (int j = 0; j < input.size(); ++j) {

      gradient[j] = (calc_sigma(input) - target) * (1 / pow(cosh(vec_prod), 2)) * input[j];
    }

    return gradient;
  }


  //update weights by gradient
  void update_weights(vector<double> input, double target, double rate) {
    
    vector<double> gradient1 = calc_gradient(input, target, true);
    vector<double> gradient2 = calc_gradient(input, target, false);

    for (int i = 0; i < w1.size(); ++i) {
      w1[i] -= rate * gradient1[i];
      w2[i] -= rate * gradient2[i];

    }
  }





  //Cost function
  double cost_E(vector<double> input, double target) {
      
    double cost = pow((calc_sigma(input) - target), 2.00);

    return cost;
  }

  //Print weights

  void print_weights(ofstream& weights) {

    weights << "w1\tw2" << endl;

    for (int i = 0; i < w1.size(); ++i) {

      weights << w1[i] << "\t" << w2[i] << endl;

    }
  }




private:
  vector<double> w1;
  vector<double> w2;

};


network::network(int dims) {

  vector<double> tempV(dims, 0.00);
  w1 = w2 = tempV;


  std::bernoulli_distribution b (0.5);
  normal_distribution<double> d1{ 1, 0.3 };

  double norm1, norm2, norm = 1.00;

  //initialize weights
  for (int i = 0; i < dims; ++i) {


    w1[i] = d1(rng);
    w2[i] = d1(rng);
    
    if(b(rng)) 
      w1[i] *= -1.0;
    if (b(rng))
      w2[i] *= -1.0;



  }

  norm1 = getVectorNorm(w1);
  norm2 = getVectorNorm(w2);


  //Normalize weights to |w| = 1
  for (int i = 0; i < dims; ++i) {

    w1[i] /= norm1;
    w2[i] /= norm2;
  }

}




int main()
{
  // create and open output stream and file
  ofstream myfile;
  myfile.open("output20-4.txt");
  myfile << "t\t" << "E\t" << "Etest\t" << endl;

  // Parameters
  double eta = 0.05;        //Learning rate
  int dims = 50;            //N input dimensions
  int trainingset_P = 500;  //training set size 
  int trainingset_Q = 500;  //testing set size
  int tmax = 1000;           //maximum training times


  //initialize network
  network ex1(dims);

  //get data sets from files labelfile.txt and trans_vfile.txt, 
  //modified from data3.mat with matlab & regular expressions
  ifstream inFile;
  inFile.open("labelfile.txt");
  if (!inFile) {
    cerr << "Unable to open file 1";
    exit(1);   // call system to stop
  }

  vector<double> labels;

  double file_ex;
  while (inFile >> file_ex)
  {
    labels.push_back(file_ex);
    //cout <<"x: "<< x << endl;
  }

  inFile.close();

  inFile.open("trans_vfile.txt");
  if (!inFile) {
    cerr << "Unable to open file 2";
    exit(1);   // call system to stop
  }

  vector<vector<double>> matrix;
  vector<double> datapoints;

  double y;
  while (inFile >> y)
  {
    if (datapoints.size() < 50) {
      datapoints.push_back(y);
      if(datapoints.size() == 50)
        matrix.push_back(datapoints);
    }
    else  {

      datapoints.clear();
      datapoints.push_back(y);


    }
  }
  //data loaded
  
  int datalength = labels.size();
  uniform_int_distribution<int> dist(0, trainingset_P);


  //start training
  for (int t = 0; t < tmax; ++t) {

    for (int p = 0; p < trainingset_P; ++p) {
      int mu = dist(rng); //select random example

      //update weights
      ex1.update_weights(matrix[mu], labels[mu], eta);

    }


    //Calculate E and Etest after each time step
    double Cost_E = 0.00;
    double Cost_Etest = 0.00;

    for (int i = 0; i < trainingset_P; ++i) {
      Cost_E += ex1.cost_E(matrix[i], labels[i]);
    }

    for (int i = 0; i < trainingset_Q; ++i) {
      Cost_Etest += ex1.cost_E(matrix[trainingset_P + i], labels[trainingset_P + i]);
    }

    Cost_E /= 2 * trainingset_P;
    Cost_Etest /= 2 * trainingset_Q;


    //write Error scores output to file
    myfile << t << "\t" << Cost_E << "\t" << Cost_Etest << endl;
  } // end training

  myfile.close();


  //write weights to file
  ofstream weights;
  weights.open("weights20-4.txt");

  ex1.print_weights(weights);

}
