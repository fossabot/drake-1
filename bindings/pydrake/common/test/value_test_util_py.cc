#include "pybind11/pybind11.h"

#include "drake/bindings/pydrake/common/value_pybind.h"
#include "drake/bindings/pydrake/pydrake_pybind.h"

namespace drake {
namespace pydrake {
namespace {

class MoveOnlyType {
 public:
  explicit MoveOnlyType(int x) : x_(x) {}
  MoveOnlyType(MoveOnlyType&&) = default;
  MoveOnlyType& operator=(MoveOnlyType&&) = default;
  MoveOnlyType(const MoveOnlyType&) = delete;
  MoveOnlyType& operator=(const MoveOnlyType&) = delete;
  int x() const { return x_; }
  void set_x(int x) { x_ = x; }
  std::unique_ptr<MoveOnlyType> Clone() const {
    return std::make_unique<MoveOnlyType>(x_);
  }

 private:
  int x_{};
};

struct UnknownType {};

}  // namespace

PYBIND11_MODULE(value_test_util, m) {
  py::module::import("pydrake.common");

  py::class_<MoveOnlyType>(m, "MoveOnlyType")
      .def(py::init<int>())
      .def("x", &MoveOnlyType::x)
      .def("set_x", &MoveOnlyType::set_x);
  // Define `Value` instantiation.
  AddValueInstantiation<MoveOnlyType>(m);

  m.def("make_unknown_abstract_value",
      []() { return AbstractValue::Make(UnknownType{}); });
}

}  // namespace pydrake
}  // namespace drake
