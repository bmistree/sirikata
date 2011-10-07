

(function()
{
    var uniqueInt = 0;
    
    function htmlEscape(str)
    {
        return str;
    }
    
    function getUniqueInt()
    {
        return ++uniqueInt;
    }

    IMUtil = {
        htmlEscape: htmlEscape,
        getUniqueInt: getUniqueInt
    };

})();
