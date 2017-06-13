

var color = 0

function Observable(initialValue)
{
    this.__color = -1;
    this.__bound = false;
    this.__observers = []; // observing this
    this.__observed = []; // observed by this
    this.__value = initialValue;
    this.__func = function() { return null; };

    Object.defineProperty(this, "value",
    {
        get: function()
        {
            return this.__value;
        },
        set: function(value)
        {
            color = color + 1
            this.__set(value)
        }
    });
}

Observable.prototype.unbind = function()
{
    var self = this;

    this.__observed.forEach(function(observable)
    {
        var index = observable.__observers.indexOf(self)
        if(index != -1)
        {
            // remove self from observers list
            observable.__observers = observable.__observers.splice(index, 1);
        }
    });

    this.__observed = [];
}

Observable.prototype.bind = function(observables, func)
{
    var self = this;
    color = color + 1;

    this.unbind()
    observables.forEach(function(observable)
    {
        if(observable.__observers.indexOf(self) == -1)
        {
            observable.__observers.push(self);
        }
    });

    this.__observed = observables;
    this.__func = func;
    this.__onUpdate();
};

Observable.prototype.__set = function(value)
{
    this.__color = color
    this.__value = value;
    this.__observers.forEach(function(observer)
    {
        observer.__onUpdate();
    });

    if(this.onUpdate)
    {
        this.onUpdate()
    }
}

Observable.prototype.__onUpdate = function()
{
    if(this.__color != color)
    {
        var values = []
        this.__observed.forEach(function(observed)
        {
            values.push(observed.value)
        })
        this.__set(this.__func.apply(this, values));
    }
};

exports.Observable = Observable;
