#include <string>
#include <fstream>
#include <sstream>

#include "spruce.hh"

#include "tinyxml.h"
#include "RigidBodyManipulator.h"
#include "joints/drakeJointUtil.h"
#include "joints/HelicalJoint.h"
#include "joints/PrismaticJoint.h"
#include "joints/RevoluteJoint.h"
#include "joints/QuaternionFloatingJoint.h"
#include "joints/RollPitchYawFloatingJoint.h"

using namespace std;

void readObjFile(spruce::path spath, vector<double>& vertex_coordinates)
{
  //spruce::path spath(fpath.toString(Poco::Path::PATH_NATIVE));

  string ext = spath.extension();
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);   
  
  ifstream file;
  if (ext.compare(".obj")==0) {
     //cout << "Loading mesh from " << fname << " (scale = " << scale << ")" << endl;
    file.open(spath.getStr().c_str(),ifstream::in);

  } else {
    spath.setExtension(".obj");
    
    if ( spath.exists() ) {
      // try changing the extension to obj and loading
      //      cout << "Loading mesh from " << mypath.replace_extension(".obj").native() << endl;
      file.open(spath.getStr().c_str(),ifstream::in);
    }
  }      

  if (!file.is_open()) {
    cerr << "Warning: Mesh " << spath.getStr() << " ignored because it does not have extension .obj (nor can I find a juxtaposed file with a .obj extension)" << endl;
  }

  string line;
  double d;
  while (getline(file,line)) {
    istringstream iss(line);
    char type;
    if (iss >> type && type == 'v') {
      while (iss >> d) {
        vertex_coordinates.push_back(d);
      }
    }
  }
}

string exec(string cmd)
{
	// from http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c
	// note: replace popen and pclose with _popen and _pclose for Windows.
	FILE* pipe = popen(cmd.c_str(), "r");
	if (!pipe) return "ERROR";
	char buffer[128];
	string result = "";
	while(!feof(pipe)) {
		if(fgets(buffer, 128, pipe) != NULL)
			result += buffer;
    }
	pclose(pipe);
	return result;
}

void searchenvvar(map<string,string> &package_map, string envvar)
{
	char* cstrpath = getenv(envvar.c_str());
	if (!cstrpath) return;

	string path(cstrpath), token, t;
	istringstream iss(path);

	while (getline(iss,token,':')) {
		istringstream p(exec("find -L "+token+" -iname package.xml"));
	  while (getline(p,t)) {
      spruce::path mypath_s(t);
      auto path_split = mypath_s.split();
      if (path_split.size() > 2) {
        
        package_map.insert(make_pair(path_split.at(path_split.size()-2), mypath_s.root().append("/")));
        //cout << mypath.getFileName() << endl;
      }
	  }
	}
}

string rospack(string package)
{
	// my own quick and dirty implementation of the rospack algorithm (based on my matlab version in rospack.m)
	static map<string,string> package_map;

	if (package_map.empty()) {
		searchenvvar(package_map,"ROS_ROOT");
		searchenvvar(package_map,"ROS_PACKAGE_PATH");
	}

	map<string,string>::iterator iter = package_map.find(package);
	if (iter != package_map.end())
		return iter->second;

	cerr << "Couldn't find package " << package << " in ROS_ROOT or ROS_PACKAGE_PATH" << endl;
	return "";
}

// todo: rectify this with findLinkId in the class (which makes more assumptions)
int findLinkIndex(RigidBodyManipulator* model, string linkname)
{
  int index = -1;
  for (unsigned int i = 0; i < model->bodies.size(); i++) {
    if (linkname.compare(model->bodies[i]->linkname) == 0) {
      index = i;
      break;
    }
  }
  return index;
}

int findLinkIndexByJointName(RigidBodyManipulator* model, string jointname)
{
  int index = -1;
  for (unsigned int i = 0; i < model->bodies.size(); i++) {
    if (model->bodies[i]->hasParent() && jointname.compare(model->bodies[i]->getJoint().getName())==0) {
      index = i;
      break;
    }
  }
  return index;
}

// only writes values if they exist
bool parseVectorAttribute(TiXmlElement* node, const char* attribute_name, Vector3d &val)
{
  const char* attr = node->Attribute(attribute_name);
  if (attr) {
    stringstream s(attr);
    s >> val(0) >> val(1) >> val(2);
    return true;
  }
  return false;
}

bool parseVectorAttribute(TiXmlElement* node, const char* attribute_name, Vector4d &val)
{
  const char* attr = node->Attribute(attribute_name);
  if (attr) {
    stringstream s(attr);
    s >> val(0) >> val(1) >> val(2) >> val(3);
    return true;
  }
  return false;
}

void poseAttributesToTransform(TiXmlElement* node, Matrix4d& T)
{
  double x = 0.0, y = 0.0, z = 0.0, roll = 0.0, pitch = 0.0, yaw = 0.0;

  const char* attr = node->Attribute("xyz");
  if (attr) {
    stringstream s(attr);
    s >> x >> y >> z;
  }

  attr = node->Attribute("rpy");
  if (attr) {
    stringstream s(attr);
    s >> roll >> pitch >> yaw;
  }

  T << cos(yaw) * cos(pitch), cos(yaw) * sin(pitch) * sin(roll) - sin(yaw) * cos(roll), cos(yaw) * sin(pitch) * cos(roll) + sin(yaw) * sin(roll), x, sin(yaw) * cos(pitch), sin(yaw) * sin(pitch) * sin(roll) + cos(yaw) * cos(roll), sin(yaw) * sin(pitch) * cos(roll) - cos(yaw) * sin(roll), y, -sin(
      pitch), cos(pitch) * sin(roll), cos(pitch) * cos(roll), z, 0, 0, 0, 1;
}

bool parseInertial(shared_ptr<RigidBody> body, TiXmlElement* node, RigidBodyManipulator* model)
{
  Isometry3d T = Isometry3d::Identity();

  TiXmlElement* origin = node->FirstChildElement("origin");
  if (origin)
    poseAttributesToTransform(origin, T.matrix());

  TiXmlElement* mass = node->FirstChildElement("mass");
  if (mass)
    mass->Attribute("value", &(body->mass));

  body->com << T(0, 3), T(1, 3), T(2, 3), 1.0;

  Matrix<double, TWIST_SIZE, TWIST_SIZE> I = Matrix<double, TWIST_SIZE, TWIST_SIZE>::Zero();
  I.block(3, 3, 3, 3) << body->mass * Matrix3d::Identity();

  TiXmlElement* inertia = node->FirstChildElement("inertia");
  if (inertia) {
    inertia->Attribute("ixx", &I(0, 0));
    inertia->Attribute("ixy", &I(0, 1));
    I(1, 0) = I(0, 1);
    inertia->Attribute("ixz", &I(0, 2));
    I(2, 0) = I(0, 2);
    inertia->Attribute("iyy", &I(1, 1));
    inertia->Attribute("iyz", &I(1, 2));
    I(2, 1) = I(1, 2);
    inertia->Attribute("izz", &I(2, 2));
  }

  auto bodyI = transformSpatialInertia(T, static_cast<Gradient<Isometry3d::MatrixType, Eigen::Dynamic>::type*>(NULL), I);
  body->I = bodyI.value();

  return true;
}

bool parseMaterial(TiXmlElement* node, map<string, Vector4d>& materials)
{
  const char* attr;
  attr = node->Attribute("name");
  if (!attr) {
    cerr << "ERROR: material tag is missing name attribute" << endl;
    return false;
  }
  string name(attr);
  auto material_iter = materials.find(name);
  bool already_in_map = false;
  if (material_iter != materials.end()) {
    already_in_map = true;
  }

  Vector4d rgba;
  TiXmlElement* color_node = node->FirstChildElement("color");
  if (color_node) {
    if (!parseVectorAttribute(color_node, "rgba", rgba)) {
      cerr << "ERROR: color tag is missing rgba attribute" << endl;
      return false;
    }
    materials[name] = rgba;
  } else if (!already_in_map) {
    cerr << "ERROR: material \"" << name << "\" is used before it is defined" << endl;
    return false;
  }
  return true;
}

bool parseGeometry(TiXmlElement* node, DrakeShapes::Element& element)
{
  const char* attr;
  TiXmlElement* shape_node;
  if ((shape_node = node->FirstChildElement("box"))) {
    double x = 0, y = 0, z = 0;
    attr = shape_node->Attribute("size");
    if (attr) {
      stringstream s(attr);
      s >> x >> y >> z;
    } else {
      cerr << "ERROR parsing box element size" << endl;
      return false;
    }
    element.setGeometry(DrakeShapes::Box(Vector3d(x, y, z)));
  } else if ((shape_node = node->FirstChildElement("sphere"))) {
    double r = 0;
    attr = shape_node->Attribute("radius");
    if (attr) {
      stringstream s(attr);
      s >> r;
    } else {
      cerr << "ERROR parsing sphere element radius" << endl;
      return false;
    }
    element.setGeometry(DrakeShapes::Sphere(r));
  } else if ((shape_node = node->FirstChildElement("cylinder"))) {
    double r = 0, l = 0;
    attr = shape_node->Attribute("radius");
    if (attr) {
      stringstream s(attr);
      s >> r;
    } else {
      cerr << "ERROR parsing cylinder element radius" << endl;
      return false;
    }

    attr = shape_node->Attribute("length");
    if (attr) {
      stringstream s(attr);
      s >> l;
    } else {
      cerr << "ERROR parsing cylinder element length" << endl;
      return false;
    }
    element.setGeometry(DrakeShapes::Cylinder(r, l));
  } else if ((shape_node = node->FirstChildElement("capsule"))) {
    double r = 0, l = 0;
    attr = shape_node->Attribute("radius");
    if (attr) {
      stringstream s(attr);
      s >> r;
    } else {
      cerr << "ERROR parsing capsule element radius" << endl;
      return false;
    }

    attr = shape_node->Attribute("length");
    if (attr) {
      stringstream s(attr);
      s >> l;
    } else {
      cerr << "ERROR parsing capsule element length" << endl;
      return false;
    }
    element.setGeometry(DrakeShapes::Capsule(r, l));
  } else if ((shape_node = node->FirstChildElement("mesh"))) {
    attr = shape_node->Attribute("filename");
    if (!attr)
      return false;
    string filename(attr);
    element.setGeometry(DrakeShapes::Mesh(filename));
  } else if ((shape_node = geometry_node->FirstChildElement("mesh-points"))) {
    attr = shape_node->Attribute("filename");
    string filename;
    if (attr) {
      filename = attr;
    } else {
      cerr << "ERROR parsing mesh element filename" << endl;
      return false;
    }

    spruce::path mesh_filename_s;
    spruce::path raw_filename_s(filename);

    auto split_filename = raw_filename_s.split();

    if (split_filename.front() == "package:") {
      spruce::path package_path_s = spruce::path(rospack(split_filename.at(2)));
      mesh_filename_s = package_path_s;

      auto split_raw = raw_filename_s.split();
      for (int i = 1; i < split_raw.size()-2; ++i) {
        mesh_filename_s.append(split_raw.at(i+2));
      }

    } else {
      mesh_filename_s = spruce::path(root_dir);
      mesh_filename_s.append(filename);
    }
    vector<double> mesh_data;
    readObjFile(mesh_filename_s, mesh_data);
    if ((mesh_data.size() > 0) && (mesh_data.size() % 3 == 0)) {
      double n_pts = mesh_data.size()/3;
      Map<Matrix3Xd> pts(mesh_data.data(),3,n_pts);
      geometry = unique_ptr<DrakeCollision::Geometry>(new DrakeCollision::Mesh(pts));
    } else {
      cerr << "ERROR parsing mesh file: " << mesh_filename_s.getStr() << endl;
      return false;
    }
    /*
     boost::shared_ptr<urdf::Mesh> mesh(boost::dynamic_pointer_cast<urdf::Mesh>(cptr->geometry));
     boost::filesystem::path mesh_filename(root_dir);
     boost::regex package(".*package://.*");
     if (!boost::regex_match(mesh->filename, package)) {
     mesh_filename /= mesh->filename;
     readObjFile(mesh_filename,params);
     } else {
     create_collision_element = false;
     if (print_mesh_package_warning) {
     cerr << "Warning: The robot '" << _urdf_model->getName()
     << "' contains collision geometries that specify mesh "
     << "files with the 'package://' syntax, which "
     << "URDFRigidBodyManipulator does not support. These "
     << "collision geometries will be ignored." << endl;
     print_mesh_package_warning = false;
     }
     }
     */
  } else {
    return false;
  }
  return true;
}

bool parseVisual(shared_ptr<RigidBody> body, TiXmlElement* node, RigidBodyManipulator* model, const map<string, Vector4d>& materials)
{
  Matrix4d T_element_to_link = Matrix4d::Identity();
  TiXmlElement* origin = node->FirstChildElement("origin");
  if (origin)
    poseAttributesToTransform(origin, T_element_to_link);

  string group_name;

  TiXmlElement* geometry_node = node->FirstChildElement("geometry");
  if (!geometry_node) {
    cerr << "ERROR: Link " << body->linkname << " has a visual element without geometry." << endl;
    return false;
  }

  DrakeShapes::VisualElement element(T_element_to_link);
  if (!parseGeometry(geometry_node, element)) {
    cerr << "ERROR: Link " << body->linkname << " has a visual element with an unknown type" << endl;
    return false;
  }

  TiXmlElement* material_node = node->FirstChildElement("material");
  if (material_node) {
    const char* attr;
    attr = node->Attribute("name");
    if (attr) {
      element.setMaterial(materials.at(attr));
    }
  }

  body->addVisualElement(element);

  return true;
}

bool parseCollision(shared_ptr<RigidBody> body, TiXmlElement* node, RigidBodyManipulator* model)
{
  Matrix4d T_element_to_link = Matrix4d::Identity();
  TiXmlElement* origin = node->FirstChildElement("origin");
  if (origin)
    poseAttributesToTransform(origin, T_element_to_link);

  const char* attr;
  string group_name;

  attr = node->Attribute("group");
  if (attr) {
    group_name = attr;
  } else {
    group_name = "default";;
  }

  TiXmlElement* geometry_node = node->FirstChildElement("geometry");
  if (!geometry_node) {
    cerr << "ERROR: Link " << body->linkname << " has a collision element without geometry" << endl;
    return false;
  }

  RigidBody::CollisionElement element(T_element_to_link, body);
  if (!parseGeometry(geometry_node, element)) {
    cerr << "ERROR: Link " << body->linkname << " has a collision element with an unknown type" << endl;
    return false;
  }

  if (element.getShape() == DrakeShapes::MESH){
    cerr << "Warning: mesh collision elements will be ignored (until I re-implement the logic below sans boost)" << endl;
    return true;
  }

  model->addCollisionElement(element, body, group_name);

  return true;
}

bool parseLink(RigidBodyManipulator* model, TiXmlElement* node, const map< string, Vector4d >& materials)
{
  const char* attr = node->Attribute("drake_ignore");
  if (attr && strcmp(attr, "true") == 0)
    return true;
  shared_ptr<RigidBody> body(new RigidBody());

  attr = node->Attribute("name");
  if (!attr) {
    cerr << "ERROR: link tag is missing name attribute" << endl;
    return false;
  }
  body->linkname = attr;

  TiXmlElement* inertial_node = node->FirstChildElement("inertial");
  if (inertial_node)
    if (!parseInertial(body, inertial_node, model))
      return false;

  for (TiXmlElement* visual_node = node->FirstChildElement("visual"); visual_node; visual_node = visual_node->NextSiblingElement("visual")) {
    if (!parseVisual(body, visual_node, model, materials)) {
      printf("error parsing visual\n");
      return false;
    }
  }

  model->bodies.push_back(body);
  body->body_index = model->bodies.size() - 1;

  for (TiXmlElement* collision_node = node->FirstChildElement("collision"); collision_node; collision_node = collision_node->NextSiblingElement("collision")) {
    if (!parseCollision(body, collision_node, model)) {
      printf("error parsing collision\n");
      return false;
    }
  }

  return true;
}

bool parseJoint(RigidBodyManipulator* model, TiXmlElement* node)
{
  const char* attr = node->Attribute("drake_ignore");
  if (attr && strcmp(attr, "true") == 0)
    return true;

  attr = node->Attribute("name");
  if (!attr) {
    cerr << "ERROR: joint tag is missing name attribute" << endl;
    return false;
  }
  string name(attr);

  attr = node->Attribute("type");
  if (!attr) {
    cerr << "ERROR: joint " << name << " is missing the type attribute" << endl;
    return false;
  }
  string type(attr);

  // parse parent
  TiXmlElement* parent_node = node->FirstChildElement("parent");
  if (!parent_node) {
    cerr << "ERROR: joint " << name << " doesn't have a parent node" << endl;
    return false;
  }
  attr = parent_node->Attribute("link");
  if (!attr) {
    cerr << "ERROR: joint " << name << " parent does not have a link attribute" << endl;
    return false;
  }
  string parent_name(attr);

  int parent_index = findLinkIndex(model, parent_name);
  if (parent_index < 0) {
    cerr << "ERROR: could not find parent link named " << parent_name << endl;
    return false;
  }

  // parse child
  TiXmlElement* child_node = node->FirstChildElement("child");
  if (!child_node) {
    cerr << "ERROR: joint " << name << " doesn't have a child node" << endl;
    return false;
  }
  attr = child_node->Attribute("link");
  if (!attr) {
    cerr << "ERROR: joint " << name << " child does not have a link attribute" << endl;
    return false;
  }
  string child_name(attr);

  int child_index = findLinkIndex(model, child_name);
  if (child_index < 0) {
    cerr << "ERROR: could not find child link named " << child_name << endl;
    return false;
  }

  Isometry3d Ttree = Isometry3d::Identity();
  TiXmlElement* origin = node->FirstChildElement("origin");
  if (origin) {
    poseAttributesToTransform(origin, Ttree.matrix());
    model->bodies[child_index]->Ttree = Ttree.matrix(); // scheduled for deletion
  }

  Vector3d axis;
  axis << 1, 0, 0;
  TiXmlElement* axis_node = node->FirstChildElement("axis");
  if (axis_node) {
    parseVectorAttribute(axis_node, "xyz", axis);
    if (axis.norm()<1e-8) {
      cerr << "ERROR: axis is zero.  don't do that" << endl;
      return false;
    }
    axis.normalize();
  }

  // todo: add damping, etc?

  TiXmlElement* limit_node = node->FirstChildElement("limit");
  if (limit_node) {
    cerr << "Warning: joint limits not (re-)implemented yet; they will be ignored." << endl;
  }

  // now construct the actual joint (based on it's type)
  DrakeJoint* joint = NULL;
  if (type.compare("revolute") == 0 || type.compare("continuous") == 0) {
    joint = new RevoluteJoint(name, Ttree, axis);
  } else if (type.compare("fixed") == 0) {
    // FIXME: implement a fixed joint class
    RevoluteJoint* rj = new RevoluteJoint(name, Ttree, axis);
    rj->setJointLimits(0, 0);
    joint = rj;
  } else if (type.compare("prismatic") == 0) {
    joint = new PrismaticJoint(name, Ttree, axis);
  } else if (type.compare("floating") == 0) {
    joint = new RollPitchYawFloatingJoint(name, Ttree);
  } else {
    cerr << "ERROR: Unrecognized joint type: " << type << endl;
    return false;
  }

  model->bodies[child_index]->setJoint(std::unique_ptr<DrakeJoint>(joint));
  model->bodies[child_index]->parent = model->bodies[parent_index];

  return true;
}

bool parseTransmission(RigidBodyManipulator* model, TiXmlElement* node)
{
  const char* attr = node->Attribute("type");
  if (!attr) {
    cerr << "ERROR: transmission element is missing the type attribute" << endl;
    return false;
  }
  string type(attr);
  if (type.find("SimpleTransmission")==string::npos) {
    cerr << "WARNING: only SimpleTransmissions are supported right now.  this element will be skipped." << endl;
    return true;
  }

  TiXmlElement* joint_node = node->FirstChildElement("joint");
  if (!joint_node || !joint_node->Attribute("name"))  {
    cerr << "ERROR: transmission is missing a joint element" << endl;
    return false;
  }
  string joint_name(joint_node->Attribute("name"));
  int body_index = findLinkIndexByJointName(model,joint_name);

  TiXmlElement* reduction_node = node->FirstChildElement("mechanicalReduction");
  double gain = 1.0;
  if (reduction_node) sscanf(reduction_node->Value(),"%lf",&gain);

  RigidBodyActuator a(joint_name,model->bodies[body_index],gain);
  model->actuators.push_back(a);
  return true;
}

bool parseLoop(RigidBodyManipulator* model, TiXmlElement* node)
{
  Vector3d ptA,ptB;

  TiXmlElement* link_node = node->FirstChildElement("link1");
  string linkname = link_node->Attribute("link");
  int bodyA = findLinkIndex(model,linkname);
  if (bodyA<0) {
    cerr << "couldn't find link %s referenced in loop joint " << linkname << endl;
    return false;
  }
  if (!parseVectorAttribute(link_node, "xyz", ptA)) {
    cerr << "ERROR parsing loop joint xyz" << endl;
    return false;
  }

  link_node = node->FirstChildElement("link2");
  linkname = link_node->Attribute("link");
  int bodyB = findLinkIndex(model,linkname);
  if (bodyB<0) {
    cerr << "couldn't find link %s referenced in loop joint " << linkname << endl;
    return false;
  }
  if (!parseVectorAttribute(link_node, "xyz", ptB)) {
    cerr << "ERROR parsing loop joint xyz" << endl;
    return false;
  }

  RigidBodyLoop l(model->bodies[bodyA],ptA,model->bodies[bodyB],ptB);
  model->loops.push_back(l);
  return true;
}

bool parseRobot(RigidBodyManipulator* model, TiXmlElement* node, const string &root_dir)
{
  printf("parseRobot\n");
  string robotname = node->Attribute("name");

  // parse material elements
  map< string, Vector4d> materials;
  for (TiXmlElement* link_node = node->FirstChildElement("material"); link_node; link_node = link_node->NextSiblingElement("material")) {
    if (!parseMaterial(link_node, materials)) {
      return false;
    }
  }
  // parse link elements
  for (TiXmlElement* link_node = node->FirstChildElement("link"); link_node; link_node = link_node->NextSiblingElement("link"))
    if (!parseLink(model, link_node, materials))
      return false;

  // todo: parse collision filter groups

  printf("a\n");
  // parse joints
  for (TiXmlElement* joint_node = node->FirstChildElement("joint"); joint_node; joint_node = joint_node->NextSiblingElement("joint"))
    if (!parseJoint(model, joint_node))
      return false;

  printf("b\n");

  // parse transmission elements
  for (TiXmlElement* transmission_node = node->FirstChildElement("transmission"); transmission_node; transmission_node = transmission_node->NextSiblingElement("transmission"))
    if (!parseTransmission(model, transmission_node))
      return false;

  printf("c\n");

  // parse loop joints
  for (TiXmlElement* loop_node = node->FirstChildElement("loop_joint"); loop_node; loop_node = loop_node->NextSiblingElement("loop_joint"))
    if (!parseLoop(model, loop_node))
      return false;

  printf("d\n");

  for (unsigned int i = 1; i < model->bodies.size(); i++) {
    if (model->bodies[i]->parent == nullptr) {  // attach the root nodes to the world with a floating base joint
      model->bodies[i]->parent = model->bodies[0];
      model->bodies[i]->setJoint(std::unique_ptr<DrakeJoint>(new RollPitchYawFloatingJoint("floating_rpy", Isometry3d::Identity())));
    }
  }
  return true;
}

bool parseURDF(RigidBodyManipulator* model, TiXmlDocument * xml_doc, const string &root_dir)
{
  TiXmlElement *node = xml_doc->FirstChildElement("robot");
  if (!node) {
    cerr << "ERROR: This urdf does not contain a robot tag" << endl;
    return false;
  }

  if (!parseRobot(model, node, root_dir))
    return false;

  model->compile();
  return true;
}

bool RigidBodyManipulator::addRobotFromURDFString(const string &xml_string, const string &root_dir)
{
  TiXmlDocument xml_doc;
  xml_doc.Parse(xml_string.c_str());
  return parseURDF(this,&xml_doc,root_dir);
}

bool RigidBodyManipulator::addRobotFromURDF(const string &urdf_filename)
{
  TiXmlDocument xml_doc(urdf_filename);
  if (!xml_doc.LoadFile()) {
    cerr << "ERROR: failed to load file " << urdf_filename << endl;
    return false;
  }

  string root_dir="";
  size_t found = urdf_filename.find_last_of("/\\");
  if (found != string::npos) {
    root_dir = urdf_filename.substr(0, found);
  }

  return parseURDF(this,&xml_doc,root_dir);
}
