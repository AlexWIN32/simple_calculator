#include <iostream>
#include <memory>
#include <optional>
#include <string>

class Argument
{
public:
    virtual float Evaluate() const = 0;
};

class Expression : public virtual Argument
{
public:
    enum class Method
    {
        NotDefined = 0,
        Add,
        Subtract,
        Divide,
        Multiply,
    };
private:
    std::unique_ptr<Argument> left;
    std::unique_ptr<Argument> right;
    Method method = Method::NotDefined;
public:
    ~Expression();
    Expression(Method Method);
    void SetLeft(std::unique_ptr<Argument> &&Left);
    void SetRight(std::unique_ptr<Argument> &&Right);
    float Evaluate() const override;
};

class Literal : public virtual Argument
{
private:
    float val;
public:
    Literal(float Value);
    float Evaluate() const override;
};

Expression::~Expression() = default;

Expression::Expression(Method Method)
    :  method(Method)
{
}

void Expression::SetLeft(std::unique_ptr<Argument> &&Left)
{
    left = std::move(Left);
}

void Expression::SetRight(std::unique_ptr<Argument> &&Right)
{
    right = std::move(Right);
}

float Expression::Evaluate() const
{
    float leftRes = left->Evaluate();
    float rightRes = right->Evaluate();

    switch(method){
    case Method::Add:
        return leftRes + rightRes;
        break;
    case Method::Subtract:
        return leftRes - rightRes;
        break;
    case Method::Divide:
        return leftRes / rightRes;
        break;
    case Method::Multiply:
        return leftRes * rightRes;
        break;
    case Method::NotDefined:
    default:
        return std::numeric_limits<float>::max();
        break;
    }
}

Literal::Literal(float Value)
    : val(Value)
{
}

float Literal::Evaluate() const
{
    return val;
}

int main(int argc, char **argv)
{
    if(argc == 1){
        std::cout << "empty expression" << std::endl;
        return 1;
    }

    std::string digitsStr = "0123456789";
    std::string operatorsStr = "+-/*";

    std::string lit;

    std::unique_ptr<Expression> last;

    auto ptr = argv[1];
    while(*ptr != '\0'){

        if(std::find(digitsStr.cbegin(), digitsStr.cend(), *ptr) != digitsStr.cend())
            lit += *ptr;

        if(std::find(operatorsStr.cbegin(), operatorsStr.cend(), *ptr) != operatorsStr.cend()){
            Expression::Method method = Expression::Method::NotDefined;

            switch(*ptr){
            case '+':
                method = Expression::Method::Add;
                break;
            case '-':
                method = Expression::Method::Subtract;
                break;
            case '/':
                method = Expression::Method::Divide;
                break;
            case '*':
                method = Expression::Method::Multiply;
                break;
            }

            if(last == nullptr){
                last = std::make_unique<Expression>(method);
                last->SetLeft(std::make_unique<Literal>(std::stof(lit)));

            }else{
                last->SetRight(std::make_unique<Literal>(std::stof(lit)));

                auto newExp = std::make_unique<Expression>(method);
                newExp->SetLeft(std::move(last));
                last = std::move(newExp);
            }

            lit = "";
        }

        ptr++;
    }

    last->SetRight(std::make_unique<Literal>(std::stof(lit)));

    if(last != nullptr)
        std::cout << "result: " << last->Evaluate() << std::endl;

    return 0;
}
