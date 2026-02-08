.pragma library

function objectify(input, objKeyProperty) {
    var result = {};
    for (var i in input) {
        if (!input.hasOwnProperty(i)) {
            continue;
        }
        var item = input[i];
        if (typeof item[objKeyProperty] === 'undefined') {
            throw new Error("The " + objKeyProperty + " does not exist in the array child with index " + i);
        }
        result[item[objKeyProperty]] = item;
    }

    return result;
}
