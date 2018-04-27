#ifndef BART_SRC_MESH_MESH_GENERATOR_H_
#define BART_SRC_MESH_MESH_GENERATOR_H_

#include <unordered_map>
#include <set>
#include <map>
#include <vector>

#include <deal.II/distributed/tria.h>
#include <deal.II/dofs/dof_handler.h>
#include <deal.II/base/parameter_handler.h>

//! This class provides functionalities to generate a distributed mesh.
/*!
 This class implement generating meshes using user-defined parameters. Supported
 functionalities are:

 (1) Genereate a coarse mesh;

 (2) Set up material IDs for all cells in coarse mesh;

 (3) Perform global refinements to the mesh.

 \author Weixiong Zheng
 \date 2017/05
 */
template <int dim>
class MeshGenerator {
 public:
  /*!
   Class constructor.

   \param prm ParameterHandler object.
   */
  MeshGenerator (dealii::ParameterHandler &prm);

  //! Class destructor.
  ~MeshGenerator ();

  /*!
   This function generates or reads in coarse mesh without global refinements.
   Currently, read-in is not fully implemented yet.

   \note There is unknown error if total number of cells in the coarse mesh
   cannot be divided by number of processors.

   \todo Add functionality to read in meshes.

   \param tria Triangulation object.
   \return Void. Modify tria in place.
   */
  void MakeGrid (dealii::Triangulation<dim> &tria);

  /*!
   Public member function to get total number of global refinements.

   \return Total number of global refinements.
   */
  int GetUniformRefinement ();

  /*!
   Public member function to get Hash table showing if a boundary is reflective.

   \return std::unordered_map with key as the boundary_id (integer) and value as
   the boolean.
   */
  std::unordered_map<int, bool> GetReflectiveBCMap ();

 private:
  /*!
   Generate initial coarse grid according to user defined parameters. The mesh
   is a hyer rectangle. In 2D, it is a rectangle. In 3D it is a cuboid. Defining
   the hyper rectangle requires two diagonal points and mesh cells per axis.

   \param tria Triangulation object.
   \return Void. Modify tria in place.
   */
  void GenerateInitialGrid (dealii::Triangulation<dim> &tria);

  /*!
   Generate initial coarse unstructured grid for pin-resolved calculations. This
   function is only valid in multi-D.

   \param tria Triangulation object.
   \return Void. Modify tria in place.
   */
  void GenerateInitialUnstructGrid (dealii::Triangulation<dim> &tria);

  /*!
   Function to generate a non-fuel pin model for 2D. 4 cells are created for the
   geometry

   \param tria Triangulation.
   \param center_xy Center of the triangulation.
   \return Void.
  */
  void NonFuelPin2DGrid (dealii::Triangulation<2> &tria,
      const dealii::Point<2> &center_xy);

  /*!
  Function to generate a coarse-mesh circle on 2D plane to represent the fuel rod.
  Depending on triangulation type, the circle is either generated by using 4
  quadrilateral cells (simple triangulation) or 4 quadrilateral cells as central
  part with a 8-cell ring surrounding the center (composite triangulation).

  \param tria Triangulation.
  \param p Center of the circle.
  \return Void.
  */
  void FuelRod2D (dealii::Triangulation<2> &tria, const dealii::Point<2> &p);

  /*!
   Function to generate a fuel pin model for 2D. 4 cells are created for the
   geometry

   \param tria Triangulation.
   \param center_xy Center of the triangulation.
   \return Void.
  */
  void FuelPin2DGrid (dealii::Triangulation<2> &tria,
      const dealii::Point<2> &center_xy);

  /*!
   This member function set up material IDs to the cells belonging to current
   processor on the coarse mesh before performing global refinements.

   \param tria Triangulation object.
   \return Void. Modify tria in place.
   */
  void InitializeMaterialID (dealii::Triangulation<dim> &tria);

  /*!
   This function set up boundary IDs. The naming philosophy is xmin->0, xmax->1,
   ymin->2, ymax->3, zmin->4, zmax->5 if boundaries are applicable.

   \param tria Triangulation object.
   \return Void. Modify tria in place.
   */
  void SetupBoundaryIDs (dealii::Triangulation<dim> &tria);

  /*!
   Function to perform global refinement to the mesh.

   For structured mesh, it acts as a wrapper invoking
   dealii::Triangulation<dim>::refine_global. For unstructured mesh, in addition,
   manifolds will first be set for cylinders in fuel pins and global refinement
   will be performed thereafter.

   \param tria Triangulation object.
   \return Void.
  */
  void GlobalRefine (dealii::Triangulation<dim> &tria);

  /*!
   This function set manifold id to all pin surfaces.

   \param tria Triangulation object.
   \return void.
  */
  void SetManifoldsAndRefine (dealii::Triangulation<dim> &tria);

  /*!
   Function to initialize the mapping: cell relative pos.->material ID on initial
   mesh.

   \param prm ParameterHandler object.
   \return Void.
   */
  void InitializeRelativePositionToIDMap (dealii::ParameterHandler &prm);

  /*!
   A function to establish the mapping: boundary id->refl. BC or not.

   \param prm ParameterHandler object.
   */
  void PreprocessReflectiveBC (dealii::ParameterHandler &prm);

  /*!
   A function to process coordinate info such as axis lengths, cell number per
   axis on initial mesh etc.

   \param prm ParameterHandler object.
   \return Void.
   */
  void ProcessCoordinateInformation (dealii::ParameterHandler &prm);

  /*!
   Get relative position of a cell by providing its center.

   \note This function will be used only when the mesh is not refined.

   \param position Cell center.
   \param relateive_position Relative position of a cell on initial coarse mesh.
   \return Void.
   */
  void GetCellRelativePosition (const dealii::Point<dim> &position,
      std::vector<int> &relative_position);

  /*!
   Boolean to determine if mesh needs to be generated or read-in. Currently, BART
   can only use generated mesh and read-in functionality is not fully developed.
   */
  bool is_mesh_generated_;
  bool is_mesh_pin_resolved_;//!< Boolean to determine if mesh is pin-resolved.
  bool have_reflective_bc_;//!< Boolean to determine if reflective BC is used.
  std::string mesh_filename_;//!< Mesh filename if mesh is read in.
  std::string rod_type_;//!< Fuel rod triangulation method.
  int global_refinements_;//!< Number of global refinements to perform.
  double rod_radius_;//!< Fuel rod radius

  //! Pin blocks having curves in 3D calculations on xy plane
  std::set<std::vector<int>> curved_blocks;

  //! Mapping: cell relative position->material ID.
  std::map<std::vector<int>, int> relative_position_to_id_;

  //! Mapping: cell relative position->fuel material ID.
  std::map<std::vector<int>, int> relative_position_to_fuel_id_;

  //! Hash table for the mapping: boundary ID->refl. boundary or not.
  std::unordered_map<int, bool> is_reflective_bc_;

  std::vector<double> axis_max_values_;//!< Max values per axis in the mesh.
  std::vector<double> cell_size_all_dir_;//!< Cell length per direction on the coarse mesh.
  std::vector<unsigned int> ncell_per_dir_;//!< Initial number of cells per axis on the coarse mesh.
};

#endif //BART_SRC_MESH_MESH_GENERATOR_H_
