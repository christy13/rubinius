require File.expand_path('../../../spec_helper', __FILE__)

describe "String::interpolate_join" do
  before(:each) do
    class String
      class <<self
        alias_method :_pristine_interpolate_join, :interpolate_join
      end
    end
  end

  after :each do
    class String
      class <<self
        remove_method :interpolate_join
        alias_method :interpolate_join, :_pristine_interpolate_join
        remove_method :_pristine_interpolate_join
      end
    end
  end

  it "returns a string according to the interpolate_join specified behavior" do
    class String
      @interpolate_args = nil
      attr_accessor :interpolate_args

      def self.interpolate_join(*s)
        ret = s.join ""
        ret.interpolate_args = s
        ret
      end
    end

    "#{1}#{2}#{3}".interpolate_args.should == ["1", "2", "3"]
    "abba#{1}#{2}#{3}hello".interpolate_args.should == 
        ["abba", "1", "2", "3", "hello"]
    "abba#{1}good#{2}bye#{3}hello".interpolate_args.should == 
        ["abba", "1", "good", "2", "bye", "3", "hello"]
  end
end
