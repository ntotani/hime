#ifndef HIME_HIME_H_
#define HIME_HIME_H_

class MyInt {
 public:
  explicit MyInt(int num);
  bool isOdd();
  bool isEven();
 private:
  int num_;
};

#define NS_HIME_BEGIN namespace hime {
#define NS_HIME_END   }

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

#endif  // HIME_HIME_H_

