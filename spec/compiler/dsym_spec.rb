require File.expand_path('../../spec_helper', __FILE__)

describe "A Dsym node" do
  relates ':"x#{(1 + 1)}y"' do
    compile do |g|
      g.push_const :String
      g.push_literal "x"

      g.push 1
      g.push 1
      g.send :+, 1, false
      g.meta_to_s

      g.push_literal "y"

      g.send_stack :interpolate_join, 3

      g.send :to_sym, 0, true
    end
  end
end
