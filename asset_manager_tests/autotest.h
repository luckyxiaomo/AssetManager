/**
 * @file autotest.h Simple helper to run multiple test cases in one application.
 * @author Rob Caldecott - https://www.blogger.com/profile/16139053405031925765
 *
 * As described on StackOverflow:
 *   https://stackoverflow.com/questions/37987301/how-to-run-multiple-qtest-classes
 *
 * With modificaition fo QApplication (to support GUI and network testing).
 *
 * Modified by Matt Selway to ensure commands from within QtCreator still work.
 */

#ifndef AUTOTEST_H
#define AUTOTEST_H

#include <QTest>
#include <QApplication>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QTimer>
#include <vector>
#include <string>
#include <iterator>

namespace AutoTest
{
typedef QList<QObject*> TestList;

inline TestList& testList()
{
  static TestList list;
  return list;
}

inline bool findObject(QObject* object)
{
  TestList& list = testList();
  if (list.contains(object))
  {
    return true;
  }
  foreach (QObject* test, list)
  {
    if (test->objectName() == object->objectName())
    {
      return true;
    }
  }
  return false;
}

inline void addTest(QObject* object)
{
  TestList& list = testList();
  if (!findObject(object))
  {
    list.append(object);
  }
}

inline int run(int argc, char *argv[])
{
  int ret = 0;

  foreach (QObject* test, testList())
  {
    ret += QTest::qExec(test, argc, argv);
  }

  return ret;
}

// The following looks quite horrible, but all it does is
// filter the list of tests to those with the function names
// specified on the command line.
inline void processTestCaseArg(int argc, char *argv[]) {
  static std::vector<std::string> argsWithValues{"-o", "-eventdelay", "-keydelay", "-mousedelay", "-maxwarnings",
                                                 "-minimumvalue", "-minimumtotal", "-iterations", "-median"};

  std::vector<std::string> arguments{};
  for (int i{0}; i < argc; ++i) {
    arguments.push_back(argv[i]);
  }

  auto i{arguments.begin()};
  ++i; // skip command name
  // skip option parameters
  while (i != arguments.end() and i->at(0) == '-') {
    auto val{*i};
    ++i;
    // skip over values
    if (std::find(argsWithValues.begin(), argsWithValues.end(), val) != argsWithValues.end()) ++i;
  }

  while (i != arguments.end()) {
    size_t pos{(*i).find(':')};
    std::string methodName{};
    if (pos == std::string::npos) methodName.append(*i);
    else methodName.append(i->substr(0, pos));
    methodName.append("()");

    for (int testI{}; testI < AutoTest::testList().size(); ) {
      if (AutoTest::testList().at(testI)->metaObject()->indexOfMethod(methodName.c_str()) < 0) {
        AutoTest::testList().removeAt(testI);
      }
      else {
        ++testI;
      }
    }

    ++i;
  }
}

} // end AutoTest

template <class T>
class Test
{
public:
  QSharedPointer<T> child;

  Test(const QString& name) : child(new T)
  {
    child->setObjectName(name);
    AutoTest::addTest(child.data());
  }
};

#define DECLARE_TEST(className) static Test<className> t(#className);

#define AUTO_TEST_MAIN \
  int main(int argc, char *argv[]) \
{ \
  QApplication app(argc, argv); \
  QObject *first{AutoTest::testList().first()}; \
  AutoTest::processTestCaseArg(argc, argv); \
  if (AutoTest::testList().empty()) { \
    AutoTest::addTest(first); \
  } \
  app.setAttribute(Qt::AA_Use96Dpi, true); \
  QTEST_SET_MAIN_SOURCE_PATH; \
  return AutoTest::run(argc, argv); \
}

// Redefine the macro as empty so that we can still use the macro in source files,
// which is necessary as that is how Qt Creator finds the test cases.
#define QTEST_APPLESS_MAIN(className)

#endif // AUTOTEST_H
