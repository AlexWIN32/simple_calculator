#include <iostream>
#include <memory>
#include <optional>
#include <stack>
#include <string>

class Argument
{
public:
    virtual float Evaluate() const = 0;
};

class Expression : public virtual Argument
{
public:
    virtual void SetLeft(std::unique_ptr<Argument> &&Left) = 0;
    virtual void SetRight(std::unique_ptr<Argument> &&Right) = 0;
};

class ArithmeticOperation : public virtual Expression
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
    ~ArithmeticOperation();
    ArithmeticOperation(Method Method);
    void SetLeft(std::unique_ptr<Argument> &&Left) override;
    void SetRight(std::unique_ptr<Argument> &&Right) override;
    float Evaluate() const override;
};

class Power : public virtual Expression
{
private:
    std::unique_ptr<Argument> left;
    std::unique_ptr<Argument> right;
public:
    ~Power();
    Power();
    void SetLeft(std::unique_ptr<Argument> &&Left) override;
    void SetRight(std::unique_ptr<Argument> &&Right) override;
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

ArithmeticOperation::~ArithmeticOperation() = default;

ArithmeticOperation::ArithmeticOperation(Method Method)
    :  method(Method)
{
}

void ArithmeticOperation::SetLeft(std::unique_ptr<Argument> &&Left)
{
    left = std::move(Left);
}

void ArithmeticOperation::SetRight(std::unique_ptr<Argument> &&Right)
{
    right = std::move(Right);
}

float ArithmeticOperation::Evaluate() const
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

Power::~Power() = default;

Power::Power() = default;

void Power::SetLeft(std::unique_ptr<Argument> &&Left)
{
    left = std::move(Left);
}

void Power::SetRight(std::unique_ptr<Argument> &&Right)
{
    right = std::move(Right);
}

float Power::Evaluate() const
{
    auto leftVal = left->Evaluate();
    auto rightVal = right->Evaluate();

    return pow(leftVal, rightVal);
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
    std::string powStr = "pow";

    std::string lit;

    std::unique_ptr<Expression> last, prev;

    std::stack<std::unique_ptr<Expression>> expressions;

    std::string parsed;

    auto ptr = argv[1];
    while(*ptr != '\0'){
        if(*ptr == '('){
            expressions.push(std::move(last));

            lit = "";
        }else if(*ptr == ')'){
            if(lit.empty() == false)
                last->SetRight(std::make_unique<Literal>(std::stof(lit)));
            else if(prev != nullptr)
                last->SetRight(std::move(prev));

            prev = std::move(last);
            last = std::move(expressions.top());
            expressions.pop();

            lit = "";
        }else if(std::find(digitsStr.cbegin(), digitsStr.cend(), *ptr) != digitsStr.cend())
            lit += *ptr;
        else if(std::find(powStr.cbegin(), powStr.cend(), *ptr) != powStr.cend()){
            lit += *ptr;

            if(lit == "pow"){

                lit = "";
            }
        }else if(*ptr == ','){
            auto newExp = std::make_unique<Power>();

            if(last != nullptr){
                if(lit.empty() == false)
                    last->SetRight(std::make_unique<Literal>(std::stof(lit)));
                else if(prev != nullptr)
                    last->SetRight(std::move(prev));

                newExp->SetLeft(std::move(last));
            }else if(prev != nullptr)
                newExp->SetLeft(std::move(prev));
            else if(lit.empty() == false)
                newExp->SetLeft(std::make_unique<Literal>(std::stof(lit)));

            last = std::move(newExp);

            lit = "";
        }else if(std::find(operatorsStr.cbegin(), operatorsStr.cend(), *ptr) != operatorsStr.cend()){
            ArithmeticOperation::Method method = ArithmeticOperation::Method::NotDefined;

            switch(*ptr){
            case '+':
                method = ArithmeticOperation::Method::Add;
                break;
            case '-':
                method = ArithmeticOperation::Method::Subtract;
                break;
            case '/':
                method = ArithmeticOperation::Method::Divide;
                break;
            case '*':
                method = ArithmeticOperation::Method::Multiply;
                break;
            }

            if(last == nullptr){
                last = std::make_unique<ArithmeticOperation>(method);
                if(lit.empty() == false)
                    last->SetLeft(std::make_unique<Literal>(std::stof(lit)));
                else if(prev != nullptr)
                    last->SetLeft(std::move(prev));
            }else{
                if(lit.empty() == false)
                    last->SetRight(std::make_unique<Literal>(std::stof(lit)));
                else if(prev != nullptr)
                    last->SetRight(std::move(prev));

                auto newExp = std::make_unique<ArithmeticOperation>(method);
                newExp->SetLeft(std::move(last));
                last = std::move(newExp);
            }

            lit = "";
        }

        parsed+=*ptr;
        ptr++;
    }

    if(last != nullptr){
        if(lit.empty() == false)
            last->SetRight(std::make_unique<Literal>(std::stof(lit)));
        else if(prev != nullptr)
            last->SetRight(std::move(prev));
    }else
        last = std::move(prev);

    if(last != nullptr)
        std::cout << "result: " << last->Evaluate() << std::endl;

    return 0;
}
