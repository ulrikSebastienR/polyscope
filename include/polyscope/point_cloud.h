#pragma once

#include "polyscope/affine_remapper.h"
#include "polyscope/color_management.h"
#include "polyscope/gl/gl_utils.h"
#include "polyscope/point_cloud_quantity.h"
#include "polyscope/polyscope.h"
#include "polyscope/standardize_data_array.h"
#include "polyscope/structure.h"

#include "polyscope/point_cloud_color_quantity.h"
#include "polyscope/point_cloud_scalar_quantity.h"
#include "polyscope/point_cloud_vector_quantity.h"

#include <vector>

namespace polyscope {

// Forward declare point cloud
class PointCloud;

// Forward declare quantity types
class PointCloudColorQuantity;
class PointCloudScalarQuantity;
class PointCloudVectorQuantity;


template <> // Specialize the quantity type
struct QuantityTypeHelper<PointCloud> {
  typedef PointCloudQuantity type;
};

class PointCloud : public QuantityStructure<PointCloud> {
public:
  // === Member functions ===

  // Construct a new point cloud structure
  template <class T>
  PointCloud(std::string name, const T& points);

  // === Overloads

  // Build the imgui display
  virtual void buildCustomUI() override;
  virtual void buildCustomOptionsUI() override;
  virtual void buildPickUI(size_t localPickID) override;

  // Render the the structure on screen
  virtual void draw() override;

  // Render for picking
  virtual void drawPick() override;

  // A characteristic length for the structure
  virtual double lengthScale() override;

  // Axis-aligned bounding box for the structure
  virtual std::tuple<glm::vec3, glm::vec3> boundingBox() override;

  virtual std::string typeName() override;

  // === Quantities

  // Scalars
  template <class T>
  PointCloudScalarQuantity* addScalarQuantity(std::string name, const T& values, DataType type = DataType::STANDARD);

  // Colors
  template <class T>
  PointCloudColorQuantity* addColorQuantity(std::string name, const T& values);

  // Vectors
  template <class T>
  PointCloudVectorQuantity* addVectorQuantity(std::string name, const T& vectors,
                                              VectorType vectorType = VectorType::STANDARD);

  // The points that make up this point cloud
  std::vector<glm::vec3> points;
  size_t nPoints() const { return points.size(); }

  // Misc data
  static const std::string structureTypeName;

  // Small utilities
  void deleteProgram();
  void writePointsToFile(std::string filename = "");
  void setPointCloudUniforms(gl::GLProgram& p);

private:
  // Visualization parameters
  glm::vec3 initialBaseColor;
  glm::vec3 pointColor;
  float pointRadius = 0.005;

  // Drawing related things
  // if nullptr, prepare() (resp. preparePick()) needs to be called
  std::unique_ptr<gl::GLProgram> program;
  std::unique_ptr<gl::GLProgram> pickProgram;

  // === Helpers
  // Do setup work related to drawing, including allocating openGL data
  void prepare();
  void preparePick();


  // === Quantity adder implementations
  PointCloudScalarQuantity* addScalarQuantityImpl(std::string name, const std::vector<double>& data, DataType type);
  PointCloudColorQuantity* addColorQuantityImpl(std::string name, const std::vector<glm::vec3>& colors);
  PointCloudVectorQuantity* addVectorQuantityImpl(std::string name, const std::vector<glm::vec3>& vectors,
                                                  VectorType vectorType);
};


// Implementation of templated constructor
template <class T>
PointCloud::PointCloud(std::string name, const T& points_)
    : QuantityStructure<PointCloud>(name), points(standardizeVectorArray<glm::vec3, 3>(points_)) {

  initialBaseColor = getNextUniqueColor();
  pointColor = initialBaseColor;
}


// Shorthand to add a point cloud to polyscope
template <class T>
void registerPointCloud(std::string name, const T& points, bool replaceIfPresent = true) {
  PointCloud* s = new PointCloud(name, points);
  bool success = registerStructure(s);
  if (!success) delete s;
}


// Shorthand to get a point cloud from polyscope
inline PointCloud* getPointCloud(std::string name = "") {
  return dynamic_cast<PointCloud*>(getStructure(PointCloud::structureTypeName, name));
}


} // namespace polyscope

#include "polyscope/point_cloud.ipp"
