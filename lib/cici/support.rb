# coding: utf-8

class Object
  def toString
    self.to_s.encode Encoding.default_external
  end
end

class String
  def toString
    self.encode Encoding.default_external
  end
end

