import compiler

class Unsafe_Source_Error(Exception):
    def __init__(self,error,descr = None,node = None):
        self.error = error
        self.descr = descr
        self.node = node
        self.lineno = getattr(node,"lineno",None)

    def __repr__(self):
        return "Line %d.  %s: %s" % (self.lineno, self.error, self.descr)
    __str__ = __repr__

class AbstractVisitor(object):
    def __init__(self):
        self._cache = {} # dispatch table

    def visit(self, node,**kw):
        cls = node.__class__
        meth = self._cache.setdefault(cls,
            getattr(self,'visit'+cls.__name__,self.default))
        return meth(node, **kw)

    def default(self, node, **kw):
        for child in node.getChildNodes():
            return self.visit(child, **kw)
    visitExpression = default

class SafeEval(AbstractVisitor):

    def visitConst(self, node, **kw):
        return node.value

    def visitDict(self,node,**kw):
        return dict([(self.visit(k),self.visit(v)) for k,v in node.items])

    def visitTuple(self,node, **kw):
        return tuple(self.visit(i) for i in node.nodes)

    def visitList(self,node, **kw):
        return [self.visit(i) for i in node.nodes]

    def visitName (self, node, **kw):
        return 'name(%s)' % node.name
        
    def visitCallFunc(self, node, **kw):
        func = self.visit (node.getChildNodes ()[0])
        args = [self.visit (i) for i in node.getChildNodes()[1:]]
        return ('call', func, args)

class SafeEvalWithErrors(SafeEval):

    def default(self, node, **kw):
        raise Unsafe_Source_Error("Unsupported source construct",
                                  node.__class__, node)

    # Add more specific errors if desired

def eval_string(source, fail_on_error = True):
    walker = fail_on_error and SafeEvalWithErrors() or SafeEval()
    try:
        ast = compiler.parse(source,"eval")
    except SyntaxError, err:
        raise
    try:
        return walker.visit(ast)
    except Unsafe_Source_Error, err:
        raise

def eval_file(filename, fail_on_error = True):
    return eval_string (open (filename).read ())

if __name__=='__main__':
    import sys
    print eval_file (sys.argv[1])