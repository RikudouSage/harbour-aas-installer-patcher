.pragma library

function values(object) {
    var result = [];
    for (var i in object) {
        if (!object.hasOwnProperty(i)) {
            continue;
        }
        result.push(object[i]);
    }

    return result;
}

function entries(object) {
    var result = [];
    for (var i in object) {
        if (!object.hasOwnProperty(i)) {
            continue;
        }

        result.push([i, object[i]]);
    }

    return result;
}
