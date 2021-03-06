#include <iostream>
#include <string>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>

// http://d.hatena.ne.jp/kikairoya/20110109/1294568213
#define TRANSFORM_STRUCT_MEMBER(ign, name, member) (decltype(name::member), member)
#define FUSION_ADAPT_STRUCT_AUTO(name, members) \
  BOOST_FUSION_ADAPT_STRUCT(name, BOOST_PP_SEQ_FOR_EACH(TRANSFORM_STRUCT_MEMBER, name, members))

// Data Structure
namespace client
{
  struct MyStruct
  {
    std::vector<int> vec;
  };
}
FUSION_ADAPT_STRUCT_AUTO(client::MyStruct, (vec));

// Grammer
namespace client
{
  namespace fusion = boost::fusion;
  namespace phoenix = boost::phoenix;
  namespace qi = boost::spirit::qi;

  template <typename Iterator, typename SkipGrammerType>
  struct Grammer : qi::grammar<Iterator, MyStruct(), SkipGrammerType>
  {
    template <typename T>
    using Rule = qi::rule<Iterator, T, SkipGrammerType>;
    
    Rule<MyStruct()> mainRule;
    Rule<int()> numberRule;
    
    Grammer() : Grammer::base_type(mainRule)
    {
      using namespace qi::labels;
      
      mainRule =
	qi::lit('(')
	>> *numberRule [phoenix::push_back(phoenix::at_c<0>(_val), _1)]
	>> qi::lit(')');
      numberRule = qi::int_;
    }
  };
}

int main()
{
  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  using Grammer = client::Grammer<std::string::const_iterator, ascii::space_type>;
  Grammer grammer;

  std::string buf;
  std::cout << "> ";
  while (std::getline(std::cin, buf)) {
    auto it = buf.cbegin();
    auto end = buf.cend();
    client::MyStruct ast;
    if (qi::phrase_parse(it, end, grammer, ascii::space, ast) && it == end) {
      for (int x : ast.vec) std::cout << x << ' ';
      std::cout << "\n> ";
    } else {
      std::string s(buf.cbegin(), it);
      std::string t(it, end);
      std::cout << s << "/ ! /" << t << "\n> ";
    }
  }
  std::cout << '\n';
}
