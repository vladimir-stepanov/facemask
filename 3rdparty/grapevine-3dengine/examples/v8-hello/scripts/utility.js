function forEach(object, cb)
{
    Object.keys(object).forEach(function(key)
    {
        cb(key, object[key])
    })
}

function print(str)
{
    grapevine.LOGI(str)
}

function indentJavascript(indentSize, jsString)
{
    var indentedString = "";
    var indentation = Array(indentSize + 1).join(" ")
    var indentLevel = 0;
    var atLinePrefix = true;

    var increaseIndent = {
            '(': true,
            '{': true,
            '[': true
        };

    var decreaseIndent = {
        ')': true,
        '}': true,
        ']': true
    }

    var whiteSpace =
    {
        ' ': true,
        '\t': true,
    }

    for(index = 0; index < jsString.length; index++)
    {
        var character = jsString[index];

        if(decreaseIndent[character])
        {
            indentLevel--;
        }

        if(atLinePrefix)
        {
            if(!whiteSpace[character])
            {
                for(indent = 0; indent < indentLevel; indent++)
                {
                    indentedString += "    ";
                }
                atLinePrefix = false;
            }
        }

        if(!atLinePrefix)
        {
            indentedString += character;
        }

        if(character == '\n')
        {
            atLinePrefix = true;
        }

        if(increaseIndent[character])
        {
            indentLevel++;
        }

    }

    return indentedString;
}

exports.print = print
exports.forEach = forEach
exports.indentJavascript = indentJavascript