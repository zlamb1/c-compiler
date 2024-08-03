#pragma once

#include <unordered_map>

enum class TokenKind : int
{
    Identifier,

    // Keywords
    Int,
    If,
    Else,
    Do,
    For,
    While,
    Break,
    Continue,
    Return,

    // Constants
    IntConstant,
    HexConstant,

    // Delimeters
    LeftParenthesis,
    RightParenthesis,
    LeftBrace,
    RightBrace,
    Colon,
    Semicolon,

    // Operators
    Minus,
    Tilde,
    Exclamation,
    Plus,
    Asterisk,
    Slash,
    Ampersand,
    DoubleAmpersand,
    Pipe,
    DoublePipe,
    Equal,
    DoubleEquals,
    NotEqual,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
    Percent,
    Caret,
    LeftShift,
    RightShift,
    PlusPlus,
    AddEquals,
    MinusMinus,
    MinusEquals,
    AsteriskEquals,
    SlashEquals,
    PercentEquals,
    LeftShiftEquals,
    RightShiftEquals,
    OrEquals,
    AndEquals,
    CaretEquals,
    Comma,
    QuestionMark,

    // Misc.
    Comment,
    MultilineComment,
    None
};

static std::unordered_map<TokenKind, std::string> TOKEN_KIND_NAMES
{
    { TokenKind::Identifier,         "Identifier" },
    { TokenKind::Int,                "int"        },
    { TokenKind::If,                 "if"         },
    { TokenKind::Else,               "else"       },
    { TokenKind::Do,                 "do"         },
    { TokenKind::For,                "for"        },
    { TokenKind::While,              "while"      },
    { TokenKind::Break,              "break"      },
    { TokenKind::Continue,           "continue"   },
    { TokenKind::Return,             "return"     },
    { TokenKind::IntConstant,        "Int"        },
    { TokenKind::HexConstant,        "0x"         },
    { TokenKind::LeftParenthesis,    "("          },
    { TokenKind::RightParenthesis,   ")"          },
    { TokenKind::LeftBrace,          "{"          },
    { TokenKind::RightBrace,         "}"          },
    { TokenKind::Colon,              ":"          },
    { TokenKind::Semicolon,          ";"          },
    { TokenKind::Minus,              "-"          },
    { TokenKind::Tilde,              "~"          },
    { TokenKind::Exclamation,        "!"          },
    { TokenKind::Plus,               "+"          },
    { TokenKind::Asterisk,           "*"          },
    { TokenKind::Slash,              "/"          },
    { TokenKind::Ampersand,          "&"          },
    { TokenKind::DoubleAmpersand,    "&&"         },
    { TokenKind::Pipe,               "|"          },
    { TokenKind::DoublePipe,         "||"         },
    { TokenKind::Equal,              "="          },
    { TokenKind::DoubleEquals,       "=="         },
    { TokenKind::NotEqual,           "!="         },
    { TokenKind::LessThan,           "<"          },
    { TokenKind::LessThanOrEqual,    "<="         },
    { TokenKind::GreaterThan,        ">"          },
    { TokenKind::GreaterThanOrEqual, ">="         },
    { TokenKind::Percent,            "%"          },
    { TokenKind::Caret,              "^"          },
    { TokenKind::LeftShift,          "<<"         },
    { TokenKind::RightShift,         ">>"         },
    { TokenKind::PlusPlus,           "++"         },
    { TokenKind::AddEquals,          "+="         },
    { TokenKind::MinusMinus,         "--"         },
    { TokenKind::MinusEquals,        "-="         },
    { TokenKind::AsteriskEquals,     "*="         },
    { TokenKind::SlashEquals,        "/="         },
    { TokenKind::PercentEquals,      "%="         },
    { TokenKind::LeftShiftEquals,    "<<="        },
    { TokenKind::RightShiftEquals,   ">>="        },
    { TokenKind::OrEquals,           "|="         },
    { TokenKind::AndEquals,          "&="         },
    { TokenKind::CaretEquals,        "^="         },
    { TokenKind::Comma,              ","          },
    { TokenKind::QuestionMark,       "?"          },
    { TokenKind::Comment,            "//"         },
    { TokenKind::None,               "None"       }
};

static bool is_keyword(TokenKind kind)
{
    switch (kind)
    {
        case TokenKind::Int:
        case TokenKind::If:
        case TokenKind::Else:
        case TokenKind::Do:
        case TokenKind::For:
        case TokenKind::While:
        case TokenKind::Break:
        case TokenKind::Continue:
        case TokenKind::Return:
            return true;
    }
    return false;
}